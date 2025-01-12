#include "semantic.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ----------------------------------------------------------------------------
// 1) Separate counters for int temps (t_0, t_1, ...) and float temps (f_0, f_1, ...)
// ----------------------------------------------------------------------------
static int intTempCounter = 0;
static int floatTempCounter = 0;

// ----------------------------------------------------------------------------
// 1.1) Label counter for generating unique labels
// ----------------------------------------------------------------------------
static int labelCounter = 0;

// Helper function to generate a unique label suffix
static int getNextLabel() {
    return labelCounter++;
}

// Generate a fresh int temp: t_0, t_1, ...
static char* newIntTemp(void) {
    char* buffer = (char*)malloc(20);
    sprintf(buffer, "t_%d", intTempCounter++);
    return buffer;
}

// Generate a fresh float temp: f_0, f_1, ...
static char* newFloatTemp(void) {
    char* buffer = (char*)malloc(20);
    sprintf(buffer, "f_%d", floatTempCounter++);
    return buffer;
}

// A small helper that picks t_ or f_ depending on DataType
static char* newTempByType(DataType dt) {
    return (dt == DataType_Float) ? newFloatTemp() : newIntTemp();
}

// ----------------------------------------------------------------------------
// 2) A helper function to figure out if a node or expression is int or float.
//    This is the core of deciding which register type to use.
// ----------------------------------------------------------------------------
static DataType getASTNodeDataType(ASTNode* node) {
    if (!node) {
        // Default fallback
        return DataType_Int;
    }

    switch (node->type) {
        case NodeType_IntExpr:
            return DataType_Int;

        case NodeType_FloatExpr:
            return DataType_Float;

        // For a binary Expression node, if either side is float => float
        case NodeType_Expr: {
            DataType leftType  = getASTNodeDataType(node->expr.left);
            DataType rightType = getASTNodeDataType(node->expr.right);
            if (leftType == DataType_Float || rightType == DataType_Float) {
                return DataType_Float;
            } else {
                return DataType_Int;
            }
        }

        // For a simple ID, array access, etc., you may want to rely on
        // a symbol table or some annotation.  For simplicity, if you've
        // annotated your AST so that a float variable is known to be NodeType_FloatExpr,
        // then you could do a simpler check.  Otherwise, you can default to int
        // or do a symbol-table lookup here if you have that data. For now:
        case NodeType_SimpleID:
        case NodeType_ArrayAccess:
        case NodeType_AssignStmt:
        case NodeType_FunctionCall:
        {
            // If in your compiler you have a proper type-check pass, 
            // you'd do a lookup here. 
            // For minimal changes, let's say "expressions are determined by their children",
            // so if we can't figure it out, default to int (or do something else).
            return DataType_Int;
        }

        // You can add more logic for arrays, if statements, etc. 
        // Typically, conditions are booleans, but we’re ignoring that
        // for now since your code is all typed as int/float.

        default:
            return DataType_Int;
    }
}

// ----------------------------------------------------------------------------
// The rest of the TAC logic remains almost identical to your original code.
// We only replaced (char* target ? target : newTemp()) with newTempByType(...)
// based on the node's data type where appropriate.
// ----------------------------------------------------------------------------

// Helper to create a TAC node
TAC* createTAC(char* operator, char* arg1, char* arg2, char* result) {
    TAC* newTAC = (TAC*)malloc(sizeof(TAC));
    newTAC->operator = operator ? strdup(operator) : NULL;
    newTAC->arg1 = arg1 ? strdup(arg1) : NULL;
    newTAC->arg2 = arg2 ? strdup(arg2) : NULL;
    newTAC->result = result ? strdup(result) : NULL;
    newTAC->next = NULL;
    return newTAC;
}

// Helper to append TAC to a list
TAC* appendTAC(TAC* head, TAC* newTAC) {
    if (!head) return newTAC;
    TAC* temp = head;
    while (temp->next) temp = temp->next;
    temp->next = newTAC;
    return head;
}

// Forward declaration for handling arguments in function calls
static TAC* generateArgs(ASTNode* args);

TAC* generateTAC(ASTNode* node, char* target) {
    if (!node) return NULL;
    TAC* code = NULL;

    switch (node->type) {
        case NodeType_Program:
            code = appendTAC(code, generateTAC(node->program.functionDefList, NULL));
            code = appendTAC(code, generateTAC(node->program.varDeclList, NULL));
            code = appendTAC(code, generateTAC(node->program.stmtList, NULL));
            break;

        case NodeType_FunctionDefList:
            code = appendTAC(code, generateTAC(node->functionDefList.functionDef, NULL));
            code = appendTAC(code, generateTAC(node->functionDefList.functionDefList, NULL));
            break;

        case NodeType_FunctionDef: {
            char funcLabel[50];
            char endFuncLabel[50];
            sprintf(funcLabel, "func_%s", node->functionDef.name);
            sprintf(endFuncLabel, "endfunc_%s", node->functionDef.name);

            code = appendTAC(code, createTAC("label", NULL, NULL, funcLabel));
            code = appendTAC(code, generateTAC(node->functionDef.varDeclList, NULL));
            code = appendTAC(code, generateTAC(node->functionDef.stmtList, NULL));
            code = appendTAC(code, createTAC("label", NULL, NULL, endFuncLabel));
            break;
        }

        case NodeType_StmtList:
            code = appendTAC(code, generateTAC(node->stmtList.stmt, NULL));
            code = appendTAC(code, generateTAC(node->stmtList.stmtList, NULL));
            break;

        case NodeType_AssignStmt: {
            /*
             * We'll assume the type of the final assigned expression
             * dictates which register we need for the RHS. 
             * (If you have a symbol table to check varName’s type, you could do that, too.)
             */
            DataType rhsType = getASTNodeDataType(node->assignStmt.expr);

            if (node->assignStmt.isArray) {
                // Generate code for array index in an int register (most likely).
                // If your language only allows int indexes, that’s easy:
                char* indexTemp = newIntTemp(); 
                code = appendTAC(code, generateTAC(node->assignStmt.arrayIndex, indexTemp));

                // Generate code for the RHS (could be float or int)
                char* rhsTemp = newTempByType(rhsType);
                code = appendTAC(code, generateTAC(node->assignStmt.expr, rhsTemp));

                // store <rhsTemp>, var[indexTemp]
                code = appendTAC(code, createTAC("store", rhsTemp, indexTemp, node->assignStmt.varName));
            } else {
                // If no array, just assign:
                char* rhsTemp = target ? strdup(target) : newTempByType(rhsType);
                code = appendTAC(code, generateTAC(node->assignStmt.expr, rhsTemp));

                // varName = <rhsTemp>
                code = appendTAC(code, createTAC("=", rhsTemp, NULL, node->assignStmt.varName));
            }
            break;
        }

        case NodeType_Expr: {
            /*
             * For a binary expression (+, -, *, /, etc.), figure out if it’s int or float:
             */
            DataType exprType = getASTNodeDataType(node);

            // Generate code for left side
            DataType leftType = getASTNodeDataType(node->expr.left);
            char* lhsTemp = newTempByType(leftType);
            code = appendTAC(code, generateTAC(node->expr.left, lhsTemp));

            // Generate code for right side
            DataType rightType = getASTNodeDataType(node->expr.right);
            char* rhsTemp = newTempByType(rightType);
            code = appendTAC(code, generateTAC(node->expr.right, rhsTemp));

            // The result register type is determined by exprType
            char* resultTemp = target ? target : newTempByType(exprType);

            // e.g. result = lhs + rhs
            code = appendTAC(code, createTAC(node->expr.operator, lhsTemp, rhsTemp, resultTemp));
            break;
        }

        case NodeType_IntExpr: {
            // Force an int register
            char* resultTemp = target ? target : newTempByType(DataType_Int);
            TAC* instr = createTAC("=", NULL, NULL, resultTemp);
            instr->arg1 = (char*)malloc(20);
            sprintf(instr->arg1, "%d", node->IntExpr.integer);
            code = appendTAC(code, instr);
            break;
        }

        case NodeType_FloatExpr: {
            // Force a float register
            char* resultTemp = target ? target : newTempByType(DataType_Float);
            TAC* instr = createTAC("=", NULL, NULL, resultTemp);
            instr->arg1 = (char*)malloc(20);
            sprintf(instr->arg1, "%f", node->FloatExpr.floatNum);
            code = appendTAC(code, instr);
            break;
        }

        case NodeType_SimpleID: {
            /*
             * If you have a type system, you'd figure out if this ID is int or float.
             * For the example, we'll assume it's int (or default). 
             * If you have a symbol table that knows 'y' is float, you might do newFloatTemp().
             */
            DataType idType = DataType_Int; 
            char* resultTemp = target ? target : newTempByType(idType);
            code = appendTAC(code, createTAC("load", node->simpleID.name, NULL, resultTemp));
            break;
        }

        case NodeType_ArrayAccess: {
            /*
             * Similarly, to properly handle float arrays, do a symbol-table lookup here.
             * For now, treat arrays as int by default. 
             */
            DataType arrType = DataType_Int;
            char* indexTemp = newTempByType(DataType_Int);
            code = appendTAC(code, generateTAC(node->arrayAccess.index, indexTemp));

            char* resultTemp = target ? target : newTempByType(arrType);
            code = appendTAC(code, createTAC("load", node->arrayAccess.arrayName, indexTemp, resultTemp));
            break;
        }

        case NodeType_WriteStmt:
            code = appendTAC(code, createTAC("write", node->writeStmt.id, NULL, NULL));
            break;

        case NodeType_IfStmt: {
            char* condTemp = newIntTemp(); // Condition often is int (0 or 1)
            code = appendTAC(code, generateTAC(node->ifStmt.condition, condTemp));

            // Generate unique suffix for labels
            int currentLabel = getNextLabel();

            // Create unique labels using the suffix
            char labelThen[20];
            char labelEnd[20];
            sprintf(labelThen, "L_then_%d", currentLabel);
            sprintf(labelEnd, "L_end_%d", currentLabel);

            // Append TAC for conditional jump to 'then' label
            code = appendTAC(code, createTAC("if", condTemp, NULL, strdup(labelThen)));

            // Handle 'else' statement if it exists
            if (node->ifStmt.elseStmt) {
                // Generate TAC for 'else' block
                code = appendTAC(code, generateTAC(node->ifStmt.elseStmt, NULL));
                // After 'else' block, jump to end label
                code = appendTAC(code, createTAC("goto", NULL, NULL, strdup(labelEnd)));
            }

            // 'then' label
            code = appendTAC(code, createTAC("label", NULL, NULL, strdup(labelThen)));
            // Generate TAC for 'then' block
            code = appendTAC(code, generateTAC(node->ifStmt.thenStmt, NULL));
            // 'end' label
            code = appendTAC(code, createTAC("label", NULL, NULL, strdup(labelEnd)));
            break;
        }

        case NodeType_ReturnStmt: {
            // Our assumption: functions only return int
            char* retTemp = newIntTemp();
            code = appendTAC(code, generateTAC(node->returnStmt.expr, retTemp));
            code = appendTAC(code, createTAC("return", retTemp, NULL, NULL));
            break;
        }

        case NodeType_VarDeclList:
            code = appendTAC(code, generateTAC(node->varDeclList.varDecl, NULL));
            code = appendTAC(code, generateTAC(node->varDeclList.varDeclList, NULL));
            break;

        case NodeType_VarDecl:
        case NodeType_ArrayDecl:
            // No TAC for variable/array declarations (just skip)
            break;

        case NodeType_FunctionCall: {
            // Generate code for each argument
            TAC* argCode = NULL;
            ASTNode* argList = node->functionCall.args;
            int argCount = 0;

            while (argList && argList->type == NodeType_StmtList) {
                ASTNode* singleArg = argList->stmtList.stmt;

                // Determine if argument is int or float
                DataType argType = getASTNodeDataType(singleArg);
                char* argTemp = newTempByType(argType);

                argCode = appendTAC(argCode, generateTAC(singleArg, argTemp));
                // param <argTemp>
                argCode = appendTAC(argCode, createTAC("param", argTemp, NULL, NULL));

                argList = argList->stmtList.stmtList;
                argCount++;
            }
            code = appendTAC(code, argCode);

            // If function returns a value and we care about it, store in target
            // We assume function returns int only (per your requirement).
            char* resultTemp = target ? target : NULL; 
            code = appendTAC(code, createTAC("call", node->functionCall.name, NULL, resultTemp));
            break;
        }

        case NodeType_WhileStmt: {
            // Generate unique suffix for labels
            int currentLabel = getNextLabel();

            // Create unique labels using the suffix
            char startLabel[25];
            char endLabel[25];
            sprintf(startLabel, "L_while_start_%d", currentLabel);
            sprintf(endLabel, "L_while_end_%d", currentLabel);

            // 'start' label
            code = appendTAC(code, createTAC("label", NULL, NULL, strdup(startLabel)));

            // Condition typically in an int register
            char* condTemp = newIntTemp();
            code = appendTAC(code, generateTAC(node->whileStmt.condition, condTemp));

            // Conditional jump to 'end' label if condition is false
            code = appendTAC(code, createTAC("ifFalse", condTemp, NULL, strdup(endLabel)));

            // Generate TAC for loop body
            TAC* bodyCode = generateTAC(node->whileStmt.body, NULL);
            if (bodyCode) {
                code = appendTAC(code, bodyCode);
            }

            // Jump back to 'start' label
            code = appendTAC(code, createTAC("goto", NULL, NULL, strdup(startLabel)));

            // 'end' label
            code = appendTAC(code, createTAC("label", NULL, NULL, strdup(endLabel)));
            break;
        }

        default:
            printf("[DEBUG] No TAC generation for node type: %s\n", nodeTypeToString(node->type));
            break;
    }

    return code;
}

void printTAC(TAC* tac) {
    TAC* current = tac;
    while (current) {
        if (current->operator && strcmp(current->operator, "=") == 0) {
            printf("%s = %s\n", current->result, current->arg1);
        } else if (current->operator && 
                  (strcmp(current->operator, "+") == 0 ||
                   strcmp(current->operator, "-") == 0 ||
                   strcmp(current->operator, "*") == 0 ||
                   strcmp(current->operator, "/") == 0)) {
            printf("%s = %s %s %s\n", current->result, current->arg1, current->operator, current->arg2);
        } else if (current->operator &&
                   (strcmp(current->operator, "==") == 0 ||
                    strcmp(current->operator, "!=") == 0 ||
                    strcmp(current->operator, "<") == 0 ||
                    strcmp(current->operator, "<=") == 0 ||
                    strcmp(current->operator, ">") == 0 ||
                    strcmp(current->operator, ">=") == 0)) {
            printf("%s = %s %s %s\n", current->result, current->arg1, current->operator, current->arg2);
        } else if (current->operator && strcmp(current->operator, "write") == 0) {
            printf("write %s\n", current->arg1);
        } else if (current->operator && strcmp(current->operator, "load") == 0) {
            if (current->arg2) {
                printf("%s = load %s[%s]\n", current->result, current->arg1, current->arg2);
            } else {
                printf("%s = load %s\n", current->result, current->arg1);
            }
        } else if (current->operator && strcmp(current->operator, "if") == 0) {
            printf("if %s goto %s\n", current->arg1, current->result);
        } else if (current->operator && strcmp(current->operator, "ifFalse") == 0) {
            printf("ifFalse %s goto %s // Exit loop if condition is false\n", current->arg1, current->result);
        } else if (current->operator && strcmp(current->operator, "goto") == 0) {
            printf("goto %s\n", current->result);
        } else if (current->operator && strcmp(current->operator, "label") == 0) {
            printf("%s:\n", current->result);
        } else if (current->operator && strcmp(current->operator, "call") == 0) {
            if (current->result) {
                printf("%s = call %s\n", current->result, current->arg1);
            } else {
                printf("call %s\n", current->arg1);
            }
        } else if (current->operator && strcmp(current->operator, "store") == 0) {
            printf("store %s, %s[%s]\n", current->arg1, current->result, current->arg2);
        } else if (current->operator && strcmp(current->operator,"return")==0) {
            printf("return %s\n", current->arg1);
        } else if (current->operator && strcmp(current->operator,"param")==0) {
            printf("param %s\n", current->arg1);
        } else {
            // Fallback
            printf("%s = %s %s %s\n",
                   current->result ? current->result : "",
                   current->arg1 ? current->arg1 : "",
                   current->operator ? current->operator : "",
                   current->arg2 ? current->arg2 : "");
        }
        current = current->next;
    }
}

void freeTAC(TAC* tac) {
    while (tac) {
        TAC* temp = tac;
        tac = tac->next;
        if (temp->operator) free(temp->operator);
        if (temp->arg1) free(temp->arg1);
        if (temp->arg2) free(temp->arg2);
        if (temp->result) free(temp->result);
        free(temp);
    }
}
