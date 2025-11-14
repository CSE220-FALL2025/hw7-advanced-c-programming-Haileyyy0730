//CSE220 HW7
//Lase edited by Hailey Zheng - 116843193

#include "hw7.h"
#include <stdbool.h>

bst_sf* insert_bst_sf(matrix_sf *mat, bst_sf *root) {
    //If root == NULL, create a new node and return it
    if (root == NULL){
        bst_sf* node = malloc(sizeof(bst_sf));
        node->mat = mat;  //Store the matrix pointer instead of copying
        node->left_child = NULL;
        node->right_child = NULL;
    }
    //Compare the matrix names to preserve BST sorted property
    if (mat->name < root->mat->name){
        root->left_child = insert_bst_sf(mat, root->left_child); //Go deeper until it finds the NULL position
    }else{
        root->right_child = insert_bst_sf(mat,root->right_child);
    }
    return root; 
}

matrix_sf* find_bst_sf(char name, bst_sf *root) {
    if (root == NULL) return NULL;
    if (name == root->mat->name) return root->mat;
    if (name < root->mat->name){
        return find_bst_sf(name, root->left_child);
    }else{
        return find_bst_sf(name, root->right_child);
    }
}

void free_bst_sf(bst_sf *root) {
    if(root == NULL) return;
    free_bst_sf(root->left_child);
    free_bst_sf(root->right_child);
    free(root->mat);
    free(root);
}

//A helper to allocate a matrix
static matrix_sf* allocate_matrix(char name, unsigned int rows, unsigned int cols){
    matrix_sf* mat = malloc(sizeof(matrix_sf) + rows * cols * sizeof(int));
    if(mat){
        mat->name = name;
        mat->num_rows = rows;
        mat->num_cols = cols;
    }
    return mat;
}

matrix_sf* add_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    if (!mat1 || !mat2) return NULL;
    if (mat1->num_rows != mat2->num_rows || mat1->num_cols != mat2->num_cols) return NULL;
    matrix_sf* result = allocate_matrix('+', mat1->num_rows, mat1->num_cols);
    if(!result) return NULL;
    int total = mat1->num_cols * mat1->num_rows;
    for (int i = 0; i < total; i++){
        result->values[i] = mat1->values[i] + mat2->values[i];
    }
    return result;
}

matrix_sf* mult_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    if (!mat1 || !mat2) return NULL;
    if (mat1->num_cols != mat2->num_rows) return NULL;
    matrix_sf* result = allocate_matrix('*',mat1->num_rows,mat2->num_cols);
    if(!result) return NULL;
    //Initialize result matrix to 0 in case of garbage value to be added
    memset(result->values, 0, mat1->num_rows * mat2->num_cols * sizeof(int));

    for (unsigned int i = 0; i < mat1->num_rows; i++){
        for (unsigned int j = 0; j < mat2->num_cols; j++){
            for (unsigned int k = 0; k < mat1->num_cols; k++){
                result->values[mat1->num_rows * i + j] += mat1->values[mat1->num_rows * i + k] * mat2->values[mat2->num_cols * k + j];
            }
        }
    }

    return result;

}

matrix_sf* transpose_mat_sf(const matrix_sf *mat) {
    if(!mat) return NULL;
    matrix_sf* result = allocate_matrix(':', mat->num_cols, mat->num_rows);
    for (unsigned int i = 0; i < mat->num_rows; i++){
        for (unsigned int j = 0; j < mat->num_cols; j++){
            result->values[i * mat->num_rows + j] = mat->values[mat -> num_cols * j + i];
        }
    }

    return result;
}

matrix_sf* create_matrix_sf(char name, const char *expr) {
    if(!expr) return NULL;
    unsigned int rows, cols;
    const char *ptr = expr;

    while (*ptr == ' ') ptr++;
    rows = strtoul(ptr, (char**)&ptr, 10);
    while (*ptr == ' ') ptr++;
    cols = strtoul(ptr, (char**)&ptr, 10);
    while (*ptr && *ptr != '[') ptr++;
    if (*ptr == '[')ptr++;
    
    matrix_sf* result = malloc(sizeof(matrix_sf) * rows * cols * sizeof(int));
    if(!result) return NULL;

    result->name = name;
    result->num_cols = cols;
    result->num_rows = rows;

    int count = 0;
    while(*ptr && *ptr != ']'){
        if (*ptr == ';' || *ptr == ' '){
            ptr++;
            continue;
        }
        int val = strtol(ptr, (char**)&ptr, 10);
        result->values[count++] = val;
    }

    return result;
}

//Stack helper functions
typedef struct{
    char* items;
    int top;
}Stack;

Stack* create_stack(int size){
    Stack* s = malloc(sizeof(Stack));
    s->items = malloc(size);
    s->top = -1;
    return s;
}

void push(Stack* s, char c){
    s->items[++(s->top)] = c;
}
char pop(Stack* s){
    return s->items[(s->top)--];
}
char peek(Stack* s){
    return s->items[s->top];
}
int is_empty(Stack* s){
    return s->top == -1;
}
void free_stack(Stack* s){
    free(s->items);
    free(s);
}
int precedence(char op){
    if (op == '\'') return 3;
    if (op == '*') return 2;
    if (op == '+') return 1;
}


char* infix2postfix_sf(char *infix) {
    if(!infix) return NULL;
    int len = strlen(infix);
    char* postfix = malloc(2 * len + 1);
    if(!postfix) return NULL;

    Stack* stack = create_stack(len);
    if(!stack){
        free(postfix);
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < len; i++){
        char c = infix[i];
        if (isspace(c)) continue;
        if (isupper(c)) postfix[j++] = c;
        else if (c == '(') push(stack,c);
        else if (c == ')') {
            while(!is_empty(stack) && peek(stack) != '('){
                postfix[j++] = pop(stack);
            }
            pop(stack); 
        }
        else if (c == '+' || c == '*' || c == '\''){
            if(c == '\''){
                postfix[j++] = c;
            }else{
                while (!is_empty(stack) && peek(stack) != '(' && precedence(peek(stack)) >= precedence(c)){
                    postfix[j++] = pop(stack);
                }
                push(stack,c);
            }
        }
    }
    while (!is_empty(stack)){
        postfix[j++] = pop(stack);
    }

    postfix[j] = '\0';
    free_stack(stack);
    char* result = realloc(postfix, j+1);
    return result ? result: postfix;
}
//helper function
//Stack for matrix pointers
typedef struct{
    matrix_sf** data;
    int top;
    int capacity;
}MatrixStack;
static MatrixStack* create_matrix_stack(int capacity){
    MatrixStack* stack = malloc(sizeof(MatrixStack));
    if(!stack) return NULL;
    stack->data = malloc(capacity*sizeof(matrix_sf*));
    if(!stack->data){
        free(stack);
        return NULL;
    }
    stack->top = -1;
    stack->capacity = capacity;
    return stack;
}
static void push_matrix(MatrixStack* stack, matrix_sf* mat){
    if(stack->top < stack->capacity - 1){
        stack->data[++stack->top] = mat;
    }
}
static matrix_sf* pop_matrix(MatrixStack* stack){
    if (stack->top >= 0){
        return stack->data[stack->top--];
    }
    return NULL;
}
static void free_matrix_stack(MatrixStack* stack){
    if(stack){
        free(stack->data);
        free(stack);
    }
}
matrix_sf* evaluate_expr_sf(char name, char *expr, bst_sf *root) {
    if (!expr || !root) return NULL;
    char* postfix = infix2postfix_sf(expr);
    if(!postfix) return NULL;
    
    int len = strlen(postfix);
    MatrixStack* stack = create_matrix_stack(len);
    if (!stack){
        free(postfix);
        return NULL;
    }
    for (int i=0; i < len; i++){
        char c = postfix[i];
        //matrix name
        if(isupper(c)){
            matrix_sf* mat = find_bst_sf(c,root);
            if(mat){
                push_matrix(stack,mat);
            }
        }
        //operator
        else if(c == '+' || c == '*' || c == '\''){
            if (c == '\''){
                //unary operator = transpose
                matrix_sf* operand = pop_matrix(stack);
                if(operand){
                    matrix_sf* result = transpose_mat_sf(operand);
                    if(result){
                        result->name = '!';
                        push_matrix(stack, result);
                        if (!isalpha(operand->name)) free(operand);
                    }
                }
            }else{
                //binary operator
                matrix_sf* right = pop_matrix(stack);
                matrix_sf* left = pop_matrix(stack);
                if(left && right){
                    matrix_sf* result = NULL;
                    if(c == '+'){
                        if (left->num_rows != right->num_rows || left->num_cols != right->num_cols) {
                            free(postfix);
                            free_matrix_stack(stack);
                            return NULL;
                        }
                        result = add_mats_sf(left, right);
                    }else if (c == '*'){
                        if (left->num_cols != right->num_rows) {
                            free(postfix);
                            free_matrix_stack(stack);
                            return NULL;
                        }
                        result = mult_mats_sf(left, right);
                    }

                    if(result){
                        result->name = '!';
                        push_matrix(stack, result);

                        //Free temporary matrices
                        if (!isalpha(left->name)) free(left);
                        if (!isalpha(right->name)) free(right);
                    }
                }
            }
        }
    }
    matrix_sf* final_result = pop_matrix(stack);
    matrix_sf* named_result = NULL;
    if(final_result){
        named_result = allocate_matrix(name, final_result->num_rows, final_result->num_cols);
        if(named_result){
            memcpy(named_result->values, final_result->values, final_result->num_rows*final_result->num_cols*sizeof(int));
        }
        //Free temporary result if it's not from BST
        if(!isalpha(final_result->name)){
            free(final_result);
        }
    }
    free_matrix_stack(stack);
    free(postfix);
    return named_result;
}
matrix_sf *execute_script_sf(char *filename) {
   if (!filename) return NULL;

   FILE* file = fopen(filename, "r");
   if(!file) return NULL;

   bst_sf* root = NULL;
   matrix_sf* last_matrix = NULL;

   char* line = NULL;
   size_t max_line_size = MAX_LINE_LEN;

   while(fgets(line, MAX_LINE_LEN, file)){
    if (strlen(line) == 0 || line[0] == '\n') continue;
    char mat_name= '\0';
    char* equals_pos = strchr(line, '=');

    if(equals_pos){
        for(char* p = line; p < equals_pos; p++){
            if(isupper(*p)){
                mat_name = *p;
                break;
            }
        }
        if(mat_name){
            char* expr_start = equals_pos +1;
            bool is_definition = false;
            for (char* p = expr_start; *p; p++){
                if(*p == '['){
                    is_definition = true;
                    break;
                }
                if(isupper(*p) || *p == '+' || *p == '*' || *p == '\'' || *p =='('){
                    break;
                }
            }
            matrix_sf* new_matrix = NULL;

            if(is_definition){
                new_matrix = create_matrix_sf(mat_name, expr_start);
            }else{
                char* newline = strchr(expr_start, '\n');
                if(newline)* newline = '\0';
                new_matrix = evaluate_expr_sf(mat_name, expr_start,root);
            }

            if(new_matrix){
                root = insert_bst_sf(new_matrix, root);
                last_matrix = new_matrix;
            }
        }
    }
   }
   if(line) free(line);
   fclose(file);

   return last_matrix;
}

// This is a utility function used during testing. Feel free to adapt the code to implement some of
// the assignment. Feel equally free to ignore it.
matrix_sf *copy_matrix(unsigned int num_rows, unsigned int num_cols, int values[]) {
    matrix_sf *m = malloc(sizeof(matrix_sf)+num_rows*num_cols*sizeof(int));
    m->name = '?';
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    memcpy(m->values, values, num_rows*num_cols*sizeof(int));
    return m;
}

// Don't touch this function. It's used by the testing framework.
// It's been left here in case it helps you debug and test your code.
void print_matrix_sf(matrix_sf *mat) {
    assert(mat != NULL);
    assert(mat->num_rows <= 1000);
    assert(mat->num_cols <= 1000);
    printf("%d %d ", mat->num_rows, mat->num_cols);
    for (unsigned int i = 0; i < mat->num_rows*mat->num_cols; i++) {
        printf("%d", mat->values[i]);
        if (i < mat->num_rows*mat->num_cols-1)
            printf(" ");
    }
    printf("\n");
}
