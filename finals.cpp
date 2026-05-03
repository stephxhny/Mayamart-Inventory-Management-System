#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTS 100
#define MAX_NAME_LENGTH 50
#define MAX_DATE_LENGTH 20
#define INPUT_BUFFER 256

// Product structure
typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    float price;
    int quantity;
    char expiration[MAX_DATE_LENGTH];
} Product;

// Global inventory
static Product inventory[MAX_PRODUCTS];
static int product_count = 0;

// Clear input buffer
static void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Get valid integer input
static int get_valid_int(const char* prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1 && value >= min && value <= max) {
            clear_input_buffer();
            return value;
        }
        printf("? Invalid! Enter %d-%d.\n", min, max);
        clear_input_buffer();
    }
}

// Get valid float input
static float get_valid_float(const char* prompt, float min) {
    float value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &value) == 1 && value >= min) {
            clear_input_buffer();
            return value;
        }
        printf("? Invalid! Positive number only.\n");
        clear_input_buffer();
    }
}

// FIXED STRING INPUT
static void get_valid_string(const char* prompt, char* buffer, int max_len) {
    printf("%s", prompt);
    
    if (fgets(buffer, max_len, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        buffer[max_len - 1] = '\0';
        
        int len = strlen(buffer);
        while (len > 0 && isspace((unsigned char)buffer[len - 1])) {
            buffer[--len] = '\0';
        }
        
        printf("? Stored: '%s'\n", buffer);
    } else {
        buffer[0] = '\0';
    }
    clear_input_buffer();
}

// **NEW: Check if name already exists (case-insensitive)**
static int name_exists(const char* name_to_check) {
    for (int i = 0; i < product_count; i++) {
        if (strcasecmp(inventory[i].name, name_to_check) == 0) {
            return 1;  // Name exists
        }
    }
    return 0;  // Name doesn't exist
}

// Get valid expiry date
static void get_valid_expiry_date(const char* prompt, char* buffer, int max_len) {
    char temp[INPUT_BUFFER];
    
    while (1) {
        printf("%s", prompt);
        if (fgets(temp, sizeof(temp), stdin) != NULL) {
            temp[strcspn(temp, "\n")] = '\0';
            
            int valid = 1;
            int len = strlen(temp);
            if (len == 0) {
                printf("? Empty! Use MM/DD/YYYY\n");
                continue;
            }
            
            for (int i = 0; i < len; i++) {
                if (!isdigit((unsigned char)temp[i]) && temp[i] != '/') {
                    valid = 0;
                    break;
                }
            }
            
            if (!valid) {
                printf("? Numbers & / only!\n");
            } else if (len >= max_len) {
                printf("? Too long!\n");
            } else {
                strncpy(buffer, temp, max_len - 1);
                buffer[max_len - 1] = '\0';
                printf("? Expiry: %s\n", buffer);
                return;
            }
        }
        clear_input_buffer();
    }
}

// Check if ID already exists
static int id_exists(int id) {
    for (int i = 0; i < product_count; i++) {
        if (inventory[i].id == id) return 1;
    }
    return 0;
}

static int find_product(int id) {
    for (int i = 0; i < product_count; i++) {
        if (inventory[i].id == id) return i;
    }
    return -1;
}

// **UPDATED: Add product with NAME validation**
static void add_product(void) {
    if (product_count >= MAX_PRODUCTS) {
        printf("\n? Inventory FULL!\n");
        return;
    }

    printf("\n=== ADD PRODUCT ===\n");
    
    // ID validation
    int id;
    do {
        id = get_valid_int("Product ID (1-9999): ", 1, 9999);
        if (id_exists(id)) {
            printf("? ID %d ALREADY EXISTS!\n", id);
        }
    } while (id_exists(id));

    inventory[product_count].id = id;
    
    // **NEW: NAME validation (case-insensitive)**
    char temp_name[MAX_NAME_LENGTH];
    do {
        get_valid_string("Product Name: ", temp_name, MAX_NAME_LENGTH);
        if (name_exists(temp_name)) {
            printf("? '%s' ALREADY EXISTS! Choose different name.\n", temp_name);
        } else {
            strncpy(inventory[product_count].name, temp_name, MAX_NAME_LENGTH - 1);
            inventory[product_count].name[MAX_NAME_LENGTH - 1] = '\0';
            break;
        }
    } while (1);
    
    // Price & Quantity
    inventory[product_count].price = get_valid_float("Unit Price ($): ", 0.01);
    inventory[product_count].quantity = get_valid_int("Quantity: ", 0, 99999);
    get_valid_expiry_date("Expiry (MM/DD/YYYY): ", inventory[product_count].expiration, MAX_DATE_LENGTH);

    product_count++;
    printf("\n? '%s' (ID:%d) ADDED SUCCESSFULLY!\n", inventory[product_count-1].name, id);
}

// View products
static void view_products(void) {
    if (product_count == 0) {
        printf("\n? No products!\n");
        return;
    }

    printf("\n=== MAYAMART INVENTORY (%d items) ===\n", product_count);
    printf("%-4s | %-25s | %8s | %5s | %10s\n", 
           "ID", "NAME", " PRICE ", " QTY ", "EXPIRES");
    printf("-------------------------------------------------------------\n");
    
    for (int i = 0; i < product_count; i++) {
        printf("%-4d | %-24.24s | $%7.2f | %4d  | %-9s\n",
               inventory[i].id,
               inventory[i].name,
               inventory[i].price,
               inventory[i].quantity,
               inventory[i].expiration);
    }
    printf("-------------------------------------------------------------\n");
}

// Update product
static void update_product(void) {
    if (product_count == 0) {
        printf("\n? No products!\n");
        return;
    }

    printf("\n=== UPDATE ===\n");
    int id = get_valid_int("Product ID: ", 1, 9999);
    int index = find_product(id);

    if (index == -1) {
        printf("? ID %d NOT FOUND!\n", id);
        return;
    }

    printf("Updating: %s (ID: %d)\n", inventory[index].name, id);
    printf("1.Price 2.Qty 3.Expiry 4.All\n");
    
    int choice = get_valid_int("Choice (1-4): ", 1, 4);
    switch (choice) {
        case 1: inventory[index].price = get_valid_float("New price ($): ", 0.01); break;
        case 2: inventory[index].quantity = get_valid_int("New qty: ", 0, 99999); break;
        case 3: get_valid_expiry_date("New expiry: ", inventory[index].expiration, MAX_DATE_LENGTH); break;
        case 4: 
            get_valid_string("New name: ", inventory[index].name, MAX_NAME_LENGTH);
            inventory[index].price = get_valid_float("New price ($): ", 0.01);
            inventory[index].quantity = get_valid_int("New qty: ", 0, 99999);
            get_valid_expiry_date("New expiry: ", inventory[index].expiration, MAX_DATE_LENGTH);
            break;
    }
    printf("\n? UPDATED!\n");
}

// Delete product
static void delete_product(void) {
    if (product_count == 0) {
        printf("\n? No products!\n");
        return;
    }

    printf("\n=== DELETE ===\n");
    int id = get_valid_int("Product ID: ", 1, 9999);
    int index = find_product(id);

    if (index == -1) {
        printf("? ID %d NOT FOUND!\n", id);
        return;
    }

    printf("Delete '%s' (Qty:%d)? [1=YES]: ", inventory[index].name, inventory[index].quantity);
    if (get_valid_int("", 0, 1)) {
        for (int i = index; i < product_count - 1; i++) {
            inventory[i] = inventory[i + 1];
        }
        product_count--;
        printf("\n? DELETED!\n");
    } else {
        printf("\n? CANCELLED!\n");
    }
}

// Search product
static void search_product(void) {
    if (product_count == 0) {
        printf("\n? No products!\n");
        return;
    }

    printf("\n=== SEARCH ===\n");
    int id = get_valid_int("Product ID: ", 1, 9999);
    int index = find_product(id);

    if (index == -1) {
        printf("? ID %d NOT FOUND!\n", id);
        return;
    }

    printf("\n?? %s\n", inventory[index].name);
    printf("   ID:%d | $%.2f | Qty:%d | %s\n", 
           inventory[index].id, inventory[index].price,
           inventory[index].quantity, inventory[index].expiration);
}

// Menu
static int show_menu(void) {
    printf("\n+--------------------------------------+\n");
    printf("|      MAYAMART INVENTORY SYSTEM       |\n");
    printf("| 1.Add  2.View  3.Search  4.Update    |\n");
    printf("| 5.Delete     6.Exit                 |\n");
    printf("+--------------------------------------+\n");
    printf("Enter choice: ");
    return get_valid_int("", 1, 6);
}

int main(void) {
    printf("?? MAYAMART Inventory System Started!\n\n");
    
    int choice;
    do {
        choice = show_menu();
        switch (choice) {
            case 1: add_product(); break;
            case 2: view_products(); break;
            case 3: search_product(); break;
            case 4: update_product(); break;
            case 5: delete_product(); break;
            case 6: printf("\n Thank you for using MAYAMART!\n"); break;
        }
        if (choice != 6) {
            printf("\n[Press Enter to continue] ");
            clear_input_buffer();
            getchar();
        }
    } while (choice != 6);

    return 0;
}


