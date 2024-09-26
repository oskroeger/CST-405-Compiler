#include "semantic.h"
#include <stdio.h>

// Perform semantic analysis on the AST
TAC* tacHead = NULL;

void semanticAnalysis(ASTNode* node, SymbolTable* symTab) {
    if (node == NULL) return;

    switch (node->type) {
         case NodeType_Program:
            printf("Performing semantic analysis on program\n");
            semanticAnalysis(node->program.varDeclList, symTab);
            semanticAnalysis(node->program.stmtList, symTab);
            break;
        case NodeType_VarDeclList:
            semanticAnalysis(node->varDeclList.varDecl, symTab);
            semanticAnalysis(node->varDeclList.varDeclList, symTab);
            break;
        case NodeType_VarDecl:
            
            // Check for redeclaration of variables
            break;
        case NodeType_StmtList:
            semanticAnalysis(node->stmtList.stmt, symTab);
            semanticAnalysis(node->stmtList.stmtList, symTab);
            break;
        case NodeType_AssignStmt:
            semanticAnalysis(node->assignStmt.expr, symTab);
            break;  
        case NodeType_Expr:
            semanticAnalysis(node->expr.left, symTab);
            semanticAnalysis(node->expr.right, symTab);
            break;
        case NodeType_BinOp:
            // Check for declaration of variables
            if (lookupSymbol(symTab, node->binOp.left->varDecl.varName) == NULL) {
                fprintf(stderr, "Semantic error: Variable %s has not been declared\n", node->varDecl.varName);
            }
            if (lookupSymbol(symTab, node->binOp.right->varDecl.varName) == NULL) {
                fprintf(stderr, "Semantic error: Variable %s has not been declared\n", node->varDecl.varName);
            }
            semanticAnalysis(node->binOp.left, symTab);
            break;
        case NodeType_SimpleID:
            // Check for declaration of variable
            if (lookupSymbol(symTab, node->simpleID.name) == NULL) {
                fprintf(stderr, "Semantic error: Variable %s has not been declared\n", node->simpleID.name);
            }
        case NodeType_SimpleExpr:
            // no checks necessary for number
        // ... handle other node types ...

        default:
            fprintf(stderr, "Unknown Node Type\n");
    }

       // ... other code ...

    if (node->type == NodeType_Expr || node->type == NodeType_SimpleExpr) {
        TAC* tac = generateTACForExpr(node);
        // Process or store the generated TAC
        printTAC(tac);
    }

    // ... other code ...

}

// You can add more functions related to semantic analysis here
// Implement functions to generate TAC expressions



TAC* generateTACForExpr(ASTNode* expr) {
    // Depending on your AST structure, generate the appropriate TAC
    // If the TAC is generated successfully, append it to the global TAC list
    // Return the generated TAC, so that it can be used by the caller, e.g. for printing
    if (!expr) return NULL;

    TAC* instruction = (TAC*)malloc(sizeof(TAC));
    if (!instruction) return NULL;

    switch (expr->type) {
        case NodeType_Expr: {
            printf("Generating TAC for expression\n");
            instruction->arg1 = createOperand(expr->expr.left);
            instruction->arg2 = createOperand(expr->expr.right);
            instruction->op = strdup("+"); //strdup(expr->expr.operator);
            instruction->result = createTempVar();
            break;
        }

        case NodeType_SimpleExpr: {
            printf("Generating TAC for simple expression\n");
            char buffer[20];
            snprintf(buffer, 20, "%d", expr->simpleExpr.number);
            instruction->arg1 = strdup(buffer);
            instruction->op = "="; //strdup(expr->expr.operator);
            instruction->arg2 = NULL;
            instruction->result = createTempVar();
            break;
        }

        case NodeType_SimpleID: {
            printf("Generating TAC for simple ID\n");
            instruction->arg1 = strdup(expr->simpleID.name);
            instruction->op = strdup("assign");
            instruction->result = createTempVar();
            break;
        }

        // Add cases for other expression types...

        default:
            free(instruction);
            return NULL;
    }

    instruction->next = NULL; // Make sure to null-terminate the new instruction

    // Append to the global TAC list
    appendTAC(&tacHead, instruction);

    return instruction;
}
// Function to create a new temporary variable for TAC
char* createTempVar() {
    static int count = 0;
    char* tempVar = malloc(10); // Enough space for "t" + number
    if (!tempVar) return NULL;
    count = allocateNextAvailableTempVar(tempVars);
    sprintf(tempVar, "t%d", count++);
    return tempVar;
}

char* createOperand(ASTNode* node) {
    // Depending on your AST structure, return the appropriate string
    // representation of the operand. For example, if the operand is a simple
    // expression or an identifier, return its string representation.
    // This function needs to be implemented based on your AST structure.
    if (!node) return NULL;

    switch (node->type) {
        case NodeType_SimpleExpr: {
            char* buffer = malloc(20);
            snprintf(buffer, 20, "%d", node->simpleExpr.number);
            return buffer;
        }

        case NodeType_SimpleID: {
            return strdup(node->simpleID.name);
        }

        case NodeType_Expr: {
            return createTempVar();
        }

        // Add cases for other operand types...

        default:
            return NULL;
    }
}

void printTAC(TAC* tac) {
    if (!tac) return;

    // Print the TAC instruction with non-null fields
    if(tac->result != NULL)
        printf("%s = ", tac->result);
    if(tac->arg1 != NULL)
        printf("%s ", tac->arg1);
    if(tac->op != NULL)
        printf("%s ", tac->op);
    if(tac->arg2 != NULL)
        printf("%s ", tac->arg2);
    printf("\n");
}

// Print the TAC list to a file
// This function is provided for reference, you can modify it as needed

void printTACToFile(const char* filename, TAC* tac) {
    FILE* file = fopen(filename , "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }   
    TAC* current = tac;
    while (current != NULL) {
        if (strcmp(current->op,"=") == 0) {
            fprintf(file, "%s = %s\n", current->result, current->arg1);
        } 
        else {
            if(current->result != NULL)
                fprintf(file, "%s = ", current->result);
            if(current->arg1 != NULL)
                fprintf(file, "%s ", current->arg1);
            if(current->op != NULL)
                fprintf(file, "%s ", current->op);
            if(current->arg2 != NULL)
                fprintf(file, "%s ", current->arg2);
            fprintf(file, "\n");
    }
        current = current->next;
    }   
    fclose(file);
    printf("TAC written to %s\n", filename);
}


// Temporary variable allocation and deallocation functions //

void initializeTempVars() {
    for (int i = 0; i < 20; i++) {
        tempVars[i] = 0;
    }
}

int allocateNextAvailableTempVar(int tempVars[]) {
   // implement the temp var allocation logic
   // use the tempVars array to keep track of allocated temp vars

    // search for the next available temp var
    for (int i = 0; i < 20; i++) {
        if (tempVars[i] == 0) {
            tempVars[i] = 1;
            return i;
        }
    }
    return -1; // No available temp var
}

void deallocateTempVar(int tempVars[], int index) {
    // implement the temp var deallocation logic
    // use the tempVars array to keep track of allocated temp vars
    if (index >= 0 && index < 20) {
        tempVars[index] = 0;
    }
}   

void appendTAC(TAC** head, TAC* newInstruction) {
    if (!*head) {
        *head = newInstruction;
    } else {
        TAC* current = *head;
        while (current->next) {
            current = current->next;
        }
        current->next = newInstruction;
    }
}
