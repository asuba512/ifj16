# Interpret jazyka IFJ16

### Příklady psaní kódu:
-modul.c:
```c
#include <stdlib.h>
#include <stdio.h>
#include "modul.h"

struct s{
    int x;
};

int main(...) {
    int a = 5, b;
    double c = 2.0;
    double *ptr;
    char string[] = "string";
    struct s S;
    struct s *Sptr;
    
    b = 10;
    ptr = &c;
    S.x = a;
    Sptr = &S;
    
    for(int i = 0; i < 5; i++) {
        if(a < b) {
            printf("Lorem ipsum. %g %d", *ptr, Sptr->x);
        }
        else {
            ...
        }
    }
    
}
-------- 1 řádek --------
/**
 * DOXY-like comments. TRY at least.
 */
int foo() {
    ...
}
-------- 1 řádek --------
int foo2(int c, char *string) {
    ...
}

```
