;; Outil de génération de programmes Pseudo-C
;; syntaxiquement corrects.
;; KEDJI Komlan Akpédjé <eric.kedji@gmail.com>
;; http://erickedji.wordpress.com/

;; WARNING: si vous apprenez Scheme, ne prenez pas exemple sur ce
;; programme. Il a été écris par un débutant (qui projette de le
;; réécrire plus proprement d'ailleurs :-) )

;; todo:
;;       manually check epsilon handling
;;       how do we control the recursion depth,
;;       so that rules like
;;       inst -> if (expr) inst
;;       does not trows the program in a infinite loop?
;;       quick-and-dirty: add a depth parameter (d)

;; ULTIMATELY: generate programs from raw BNF descriptions

; profondeur maximale de dérivation
; si on la dépasse, on force la dérivation
; sur les branches sécurisées (ie qui sont
; guaranties aboutir en un nombre fini
; d'étapes à un terminal)
(define *max-depth* 3)

; ex: <liste-instructions> ::= <instruction>; <liste-instructions>
; Nombre maximum d'<instruction> a générer dans un tel cas.
(define *max-sequence-length* 10)

; string-append, ça fait trop long :-)
(define-syntax sa
  (syntax-rules ()
    ((_ s1 s2 ...)
     (string-append s1 s2 ...))))

; cette macro nous évite d'évaluer les alternatives avant d'en
; choisir une (c'est un gain en performance)
; bug: les alternatives notées en premier ont plus de chance
; d'être choisies.
(define-syntax choose-from
  (syntax-rules ()
    ((_ e)
     e)
    ((_ e1 e2 ...)
     (if (= (random 2) 0)
         e1
         (choose-from e2 ...)))))

; quoi de plus naturel qu'un define production
; pour définir une production? :-)
(define-syntax define-production
  (syntax-rules (safe-choices unsafe-choices)
    ((_
      (production-name current-depth)
      (safe-choices sc1 sc2 ...)
      (unsafe-choices uc1 uc2 ...))
     (define (production-name current-depth)
       (set! current-depth (+ 1 current-depth))
       (if (> current-depth *max-depth*)
           (choose-from sc1 sc2 ...)
           (choose-from uc1 uc2 ... sc1 sc2 ...))))
    ((_ (production-name current-depth) c1 c2 ...)
     (define (production-name current-depth) (choose-from c1 c2 ...)))))

; Obtention de plusieurs occurences d'une même production
(define (list-of rule separator current-depth)
  (define (lo-helper how-many)
    (if (= 0 how-many)
        (rule (+ 1 current-depth))
        (string-append (rule (+ 1 current-depth))
                       separator
                       (lo-helper (- how-many 1)))))
  (lo-helper (random *max-sequence-length*)))

(define (pseudo-c-program)
  (sa (list-of declaration " " 0) ; current depth is 0
      " "
      (list-of function " " 0)
      " "))

(define-production (declaration d)
  (sa "int " (identifier d) ";")
  (sa "int " (identifier d) "[" (int d) "];")
  (sa "static int " (identifier d) ";")
  (sa "static int " (identifier d) "[" (int d) "];")
  (sa "void " (identifier d) "(void);")
  (sa "int "
      (identifier d)
      "("
      (list-of type "," d)
      ")" ";"))

(define-production (type d)
  "int"
  (sa "int[" (int d) "]"))


(define-production (function d)
  (sa (function-header d) " " (function-body d)))

(define-production (function-header d)
  (sa "void " (identifier d) "(void)")
  (sa "int " (identifier d) "(int " (identifier d)
      ",int " (identifier d) "[" (int d) "])")
  (sa "int " (identifier d) "(int " (identifier d) "[" (int d) "])"))

(define-production (function-body d)
  (sa "{"
      (list-of declaration " " d)
      " "
      (list-of instruction " " d)
      "}"))

(define-production (instruction d)
  (safe-choices
   (sa (identifier d) "=" (expr d) ";")
   (sa (identifier d) "[" (expr d) "]=" (expr d) ";")
   (sa "exit(" (int d) ");")
   (sa "return " (expr d) ";")
   "return;"
   "write_string(\"Hello World\");"
   (sa "write_int(" (expr d) ");")
   (sa "write_int(" (list-of expr "," d) ");")
   (sa "read_int " (identifier d) ";")
   (sa (expr d) ";"))
  (unsafe-choices
   (sa "if(" (expr d) ")" (instruction d) "else{" (list-of instruction " " d) "}")
   (sa "if(" (expr d) ")" "{" (list-of instruction " " d) "}")
   (sa "while(" (expr d) ")" (instruction d))
   (sa "while(" (expr d) "){" (list-of instruction " " d) "}")
   (sa "{" (list-of instruction " " d) "}")))

(define-production (expr d)
  (safe-choices
   (sa (unary-op d) "42")
   (sa "( 42 > " (identifier d) "() " (binary-op d) (identifier d) " > " (int d) ")")
   (identifier d)
   (int d))
  (unsafe-choices
   (sa (expr d) (binary-op d) (expr d))
   (sa (identifier d) (binary-op d) (int d))
   (sa (unary-op d) (expr d))))
  

(define-production (binary-op d)
  ">" "<" "==" "<=" ">=" "!=" "+" "-" "*" "/" "%" "||" "&&")

(define-production (unary-op d)
  "-" "!")

(define-production (identifier d)
  "foo" "bar" "baz" "egg" "segfault" "rtfm"
  "nerd" "guru" "hacker" "snafuced" "i" "j"
  "k" "l" "m" "n" "pipe" "socket" "stuff"
  "thing" "sigfpe" "bad")

(define-production (int d)
  (number->string (random 1000000)))

(display (pseudo-c-program))

