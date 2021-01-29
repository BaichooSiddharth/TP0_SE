#include <stdlib.h>
#include <stdio.h>
#include "main.h"


typedef unsigned char byte;
typedef int error_code;

#define ERROR (-1)
#define HAS_ERROR(code) ((code) < 0)
#define HAS_NO_ERROR(code) ((code) >= 0)

/**
 * Cette fonction compare deux chaînes de caractères.       
 * @param p1 la première chaîne
 * @param p2 la deuxième chaîne
 * @return le résultat de la comparaison entre p1 et p2. Un nombre plus petit que
 * 0 dénote que la première chaîne est lexicographiquement inférieure à la deuxième.
 * Une valeur nulle indique que les deux chaînes sont égales tandis qu'une valeur positive
 * indique que la première chaîne est lexicographiquement supérieure à la deuxième.
 */
int strcmp(char *p1, char *p2) {
    char *s1 = (char *) p1;
    char *s2 = (char *) p2;
    char c1, c2;
    do {
        c1 = (char) *s1++;
        c2 = (char) *s2++;
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}

/**
 * Ex. 1: Calcul la longueur de la chaîne passée en paramètre selon
 * la spécification de la fonction strlen standard
 * @param s un pointeur vers le premier caractère de la chaîne
 * @return le nombre de caractères dans le code d'erreur, ou une erreur
 * si l'entrée est incorrecte
 */
error_code strlen2(char *s) {
    int i = 0;
    while(s[i] != '\0'){
        i++;
    }
    return i;
}

/**
 * Ex.2 :Retourne le nombre de lignes d'un fichier sans changer la position
 * courante dans le fichier.
 * @param fp un pointeur vers le descripteur de fichier
 * @return le nombre de lignes, ou -1 si une erreur s'est produite
 */
error_code no_of_lines(FILE *fp) {
    int no_lines = 1;
    fpos_t pos;
    fgetpos(fp, &pos);
    int char_read = fgetc(fp);
    if (fp == NULL){
        no_lines = -1;
    } else if(char_read == EOF){
        no_lines = 0;
    } else {
        while (char_read != EOF){
            if (char_read == '\n'){
                no_lines++;
            }
            char_read = fgetc(fp);
        }
    }
    fsetpos(fp, &pos);
    return no_lines;
}

/**
 * Ex.3: Lit une ligne au complet d'un fichier
 * @param fp le pointeur vers la ligne de fichier
 * @param out le pointeur vers la sortie
 * @param max_len la longueur maximale de la ligne à lire
 * @return le nombre de caractère ou ERROR si une erreur est survenue
 */
error_code readline(FILE *fp, char **out, size_t max_len) {
    int no_chars = 0;
    char *line = malloc(sizeof(char) * max_len);
    if (!fp) {
        return ERROR;
    } else {
        char car = fgetc(fp);
        while (car != EOF && car != '\n') {
            line[no_chars] = car;
            no_chars++;
            car = fgetc(fp);
        }
        line[no_chars] = '\0';
        *out = line;
    }
    return no_chars;
}

/**
 * Ex.4: Copie un bloc mémoire vers un autre
 * @param dest la destination de la copie
 * @param src  la source de la copie
 * @param len la longueur (en byte) de la source
 * @return nombre de bytes copiés ou une erreur s'il y a lieu
 */
error_code memcpy2(void *dest, void *src, size_t len) {
    //code inspiré de geeksforgeeks.org/write-memcpy
    //commentaire pour commit
    char *d = dest;
    char *s = src;
    int no_bytes = 0;
    for(int i=0; i<len; i++){
        d[i] = s[i];
        no_bytes++;
    }
    return no_bytes;
}

/**
 * Ex.5: Analyse une ligne de transition
 * @param line la ligne à lire
 * @param len la longueur de la ligne
 * @return la transition ou NULL en cas d'erreur
 */
transition *parse_line(char *line, size_t len) {
    //Si la ligne n'est pas une transition, on retourne null
    if (len < 3) {
        return NULL;
    } else {

        transition *resultat = malloc( sizeof(transition) );
        if(!resultat){
            return NULL;
        }
        int head = 1; //on skip le premier char, qui est un '('
        int tail;

        for( tail = 2 ; tail <= 6 ; tail++){
            if( *(line+tail) == ',' ){
                char *current_state_read = malloc(sizeof(char)*(tail-head+1));
                if(!current_state_read){
                    return NULL;
                }
                //le -1 c'est pour ne pas compter le char ','
                current_state_read[tail-head] = '\0';
                memcpy2(current_state_read,(line+head),tail-head);
                resultat->current_state = current_state_read;
                break;
            }
        }

        tail++; //on passe au prochain char "read"
        resultat->read = line[tail];

        tail += 5;
        head = tail;

        for( tail++ ; tail-head <= 5 ; tail++){
            if( *(line+tail) == ',' ){
                char *next_state_read = malloc(sizeof(char)*(tail-head+1));
                if(!next_state_read){
                    return NULL;
                }
                //le -1 c'est pour ne pas compter le char ','
                next_state_read[tail-head] = '\0';
                memcpy2(next_state_read,(line+head),tail-head);
                resultat->next_state = next_state_read;
                break;
            }
        }

        tail++;
        resultat->write = line[tail];

        tail += 2;
        char move;
        if(line[tail] == 'D') {
            move = 1;
        }else if(line[tail] == 'G'){
            move = -1;
        } else {
            move = 0;
        }
        resultat->movement = move;

        return resultat;

    }
}

/**
 * Ex.6: Execute la machine de turing dont la description est fournie
 * @param machine_file le fichier de la description
 * @param input la chaîne d'entrée de la machine de turing
 * @return le code d'erreur
 */
error_code execute(char *machine_file, char *input) {
    FILE *fp = fopen(machine_file, "r");
    int no_lines = no_of_lines(fp);
    transition *table_transition[no_lines-3];
    char **str_temp = malloc(sizeof(char *));
    for (int i=0; i<no_lines; i++){
        readline(fp, str_temp, 19);
        if(i<3){
            continue;
        } else {
            int len = strlen2(*str_temp);
            table_transition[i-3] = parse_line(*str_temp, len);
        }
    }
    fclose(fp);
    return ERROR;
}

// ATTENTION! TOUT CE QUI EST ENTRE LES BALISES ༽つ۞﹏۞༼つ SERA ENLEVÉ! N'AJOUTEZ PAS D'AUTRES ༽つ۞﹏۞༼つ

// ༽つ۞﹏۞༼つ

int main() {
// ous pouvez ajouter des tests pour les fonctions ici
    char *temp = "22";
    printf("%d", strlen2(temp));

    printf("\n");
    FILE *fp = fopen("../five_lines", "r");
    printf("%d", no_of_lines(fp));
    fclose(fp);


    printf("\n");
    FILE *fp2 = fopen("../six_lines", "r");
    printf("%d", no_of_lines(fp2));
    fclose(fp2);


    printf("\n");
    char **str = malloc(sizeof (char*));
    FILE *fp3 = fopen("../five_lines", "r");
    printf("%d", readline(fp3, str, 10));
    printf("\n");
    printf("%s", *str);
    fclose(fp3);


    printf("\n");
    char *before = malloc(sizeof(char) * 19);
    char *after =  "bbbbbbbbbbbbbbbbb";
    memcpy2(before, after, 19);
    printf("%s", before);
    free(before);

    printf("\n");
    char **str2 = malloc(sizeof (char*));
    FILE *fp4 = fopen("../youre_gonna_go_far_kid", "r");
    void *new_pointer = fp4+20;
    fpos_t pos;
    fgetpos(fp4, &pos);
    readline(fp4, str2, 19);
    readline(fp4, str2, 19);
    readline(fp4, str2, 19);
    readline(fp4, str2, 19);
    printf("%s", *str2);
    printf("\n");
    readline(fp4, str2, 19);
    printf("%s", *str2);
    printf("\n");
    readline(fp4, str2, 19);
    printf("%s", *str2);
    printf("\n");
    readline(fp4, str2, 19);
    printf("%s", *str2);
    printf("\n");
    transition *tr = parse_line(*str2, 17);
    fclose(fp4);

    printf("\n");
    execute("../youre_gonna_go_far_kid", "10");
    return 0;




}

// ༽つ۞﹏۞༼つ