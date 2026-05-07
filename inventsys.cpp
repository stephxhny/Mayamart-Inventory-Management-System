/*
Name : Stephanie Clacio, Ma.Princess Marticio BSIT-2E
Subject : CC105- Data Structure & Algorithm 
FINAL ASSSESSMENT - Mayamart Inventory Management System
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_PRODUCTS 100
#define MAX_NAME_LENGTH 50
#define MAX_DATE_LENGTH 20
#define INPUT_BUFFER 256

// ==================== PRODUCT STRUCTURE
typedef struct {							
    int id; //--product id							 
    char name[50]; //--product name (no numbers allowed!)		   
    float price; //--unit price (peso)					   
    int quantity; //-- stock count 
    char expiration[20]; //--mm/dd/yyyy date format
} Product;

static Product inventory[100]; //-- fixed array 
static int product_count = 0; //-- current count

// ==================== DATE VALIDATION FUNCTIONS
static int is_valid_date(int month, int day, int year) {
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    
    int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // February leap year check
    if (month == 2) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            days_in_month[2] = 29;
        }
    }
    
    return day <= days_in_month[month];
}

static int get_today_date_num(void) {	//--gets the exact month, day and year today 
    time_t t = time(NULL);
    struct tm* today = localtime(&t);
    int month = today->tm_mon + 1;
    int day = today->tm_mday;
    int year = today->tm_year + 1900;
    return year * 10000 + month * 100 + day;
}

static int date_to_number(const char* date_str) {	//--
    int month, day, year;
    if (sscanf(date_str, "%2d/%2d/%4d", &month, &day, &year) == 3) {
        return year * 10000 + month * 100 + day;
    }
    return -1;
}

static int is_valid_expiry_date(const char* date_str) {   //-- methods for valid expiry date
    int month, day, year;

    // STRICT FORMAT: MM/DD/YYYY
    if (sscanf(date_str, "%2d/%2d/%4d", &month, &day, &year) != 3)
        return 0;

    // VALID CALENDAR DATE
    if (!is_valid_date(month, day, year))
        return 0;

    int date_num = date_to_number(date_str);
    int today_num = get_today_date_num();

    // MUST BE TODAY OR FUTURE
    return date_num >= today_num;
}

// ==================== NAME VALIDATION FUNCTION 
static int is_valid_product_name(const char* name) {  //-- methods for valid product name
    int length = strlen(name);
    if (length == 0 || length >= 50 - 1) return 0;

    for (int i = 0; i < length; i++) {
        char c = name[i];
        if (!isalpha(c) && !isspace(c) && c != '-' && c != '\'' && c != '.' && c != ',') {
            return 0;
        }
    }
    return 1;
}

// ==================== SORTING ALGORITHMS
static int compare_dates(const char* date1, const char* date2) {
    return date_to_number(date1) - date_to_number(date2);
}
//-- Bubble sort - Items sort by Expiry Date -- soonest expiry first 
static void bubble_sort_expiry(Product arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (compare_dates(arr[j].expiration, arr[j + 1].expiration) > 0) {
                Product temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
//-- Insertion sort - Items sort by the unit price -- lowest price first 
static void insertion_sort_price(Product arr[], int n) {
    for (int i = 1; i < n; i++) {
        Product key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].price > key.price) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// ==================== SEARCHING ALGORITHMS
//-- Linear search - Items search by ID & Name -- scans entire array sequentially 
static int linear_search_id(Product arr[], int n, int id) {
    for (int i = 0; i < n; i++) {
        if (arr[i].id == id) return i;
    }
    return -1;
}

static int linear_search_name(Product arr[], int n, const char* name) {
    for (int i = 0; i < n; i++) {
        if (strcasecmp(arr[i].name, name) == 0) return i;
    }
    return -1;
}

// ==================== UTILITY FUNCTIONS
static void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int get_valid_int(const char* prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1 && value >= min && value <= max) {
            clear_input_buffer();
            return value;
        }
        printf(" Invalid! Enter %d-%d.\n", min, max);
        clear_input_buffer();
    }
}

static float get_valid_float(const char* prompt, float min) {
    float value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &value) == 1 && value >= min) {
            clear_input_buffer();
            return value;
        }
        printf(" Invalid! Positive number only.\n");
        clear_input_buffer();
    }
}

static void get_valid_product_name(const char* prompt, char* buffer, int max_length) {
    char temp[50]; //--max name_length

    printf("\n%s", prompt);
    printf("(Letters, spaces, -, ', ., , only - NO NUMBERS!)\n");

    while (1) {
        if (fgets(temp, sizeof(temp), stdin) != NULL) {
            temp[strcspn(temp, "\n")] = '\0';
            temp[max_length - 1] = '\0';

            // Trim trailing spaces
            int length = strlen(temp);
            while (length > 0 && isspace((unsigned char)temp[length - 1])) {
                temp[--length] = '\0';
            }

            if (!is_valid_product_name(temp)) {
                printf("INVALID NAME! NO NUMBERS ALLOWED!\n");
                printf("Examples: 'Milk', 'Bread Fresh', 'Apple-Pie'\n");
                printf("Enter again: ");
                continue;
            }
            strncpy(buffer, temp, max_length - 1);
            buffer[max_length - 1] = '\0';
            printf(" VALID NAME: '%s'\n", buffer);
            return;
        }
        clear_input_buffer();
    }
}

static void get_valid_expiry_date(const char* prompt, char* buffer, int max_length) {
    char temp[INPUT_BUFFER];
    int today_num = get_today_date_num();
    int month = (today_num / 100) % 100;
    int day = today_num % 100;
    int year = today_num / 10000;

    printf("\nTODAY: %02d/%02d/%04d\n", month, day, year);

    while (1) {
        printf("%s", prompt);
        if (fgets(temp, sizeof(temp), stdin) != NULL) {
            temp[strcspn(temp, "\n")] = '\0';

            if (!is_valid_expiry_date(temp)) {
                printf("INVALID DATE! Enter valid date (MM/DD/YYYY) TODAY or FUTURE.\n");
                continue;
            }

            strncpy(buffer, temp, max_length - 1);
            buffer[max_length - 1] = '\0';
            printf(" VALID EXPIRY: %s\n", buffer);
            return;
        }
        clear_input_buffer();
    }
}

// ==================== INVENTORY FUNCTIONS 
static int name_exists(const char* name_to_check) {
    for (int i = 0; i < product_count; i++) {
        if (strcasecmp(inventory[i].name, name_to_check) == 0) return 1;
    }
    return 0;
}

static int id_exists(int id) {
    return linear_search_id(inventory, product_count, id) != -1;
}

static int find_product(int id) {
    return linear_search_id(inventory, product_count, id);
}

static void add_product(void) {
    if (product_count >= MAX_PRODUCTS ) { // only allows 100 items!
        printf("\nInventory FULL!\n");
        return;
    }

    printf("\n=== ADD PRODUCT ===\n");

    // ID Validation
    int id;
    do {
        id = get_valid_int("Enter Product ID (1-9999): ", 1, 9999);
        if (id_exists(id)) printf(" ID %d ALREADY EXISTS!\n", id);
    } while (id_exists(id));
    inventory[product_count].id = id;

    // Name validation
    char temp_name[MAX_NAME_LENGTH];
    do {
        get_valid_product_name("Enter Product Name: ", temp_name, MAX_NAME_LENGTH);
        if (name_exists(temp_name)) {
            printf(" '%s' ALREADY EXISTS! Try another name.\n", temp_name);
        } else {
            strncpy(inventory[product_count].name, temp_name, MAX_NAME_LENGTH - 1);
            inventory[product_count].name[MAX_NAME_LENGTH - 1] = '\0';
            break;
        }
    } while (1);

    // Price, Quantity, Expiry
    inventory[product_count].price = get_valid_float("Enter Unit Price (P): ", 0.01);
    inventory[product_count].quantity = get_valid_int("Enter Quantity: ", 0, 99999);
    get_valid_expiry_date("Enter Expiry Date (MM/DD/YYYY): ", inventory[product_count].expiration, MAX_DATE_LENGTH);

    product_count++;
    printf("\n '%s' (ID:%d) ADDED SUCCESSFULLY!\n", inventory[product_count-1].name, id);
}

static void view_products(void) {
    if (product_count == 0) {
        printf("\nNo products added.\n");
        return;
    }

    printf("\n=== UNSORTED INVENTORY (%d items) ===\n", product_count);
    printf("%-4s | %-25s | %8s | %5s | %10s\n", "PRODUCT ID", "PRODUCT NAME", "UNIT PRICE", "QUANTITY", "EXPIRY DATE");
    printf("-----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < product_count; i++) {
        printf("%-4d | %-24.24s | P%7.2f | %4d | %-9s\n",
               inventory[i].id, inventory[i].name, inventory[i].price,
               inventory[i].quantity, inventory[i].expiration);
    }
    printf("-----------------------------------------------------------------------------------------\n");
}

static void view_products_expiry_sorted(void) {
    if (product_count == 0) return;

    Product sorted[MAX_PRODUCTS];
    memcpy(sorted, inventory, product_count * sizeof(Product));

    printf("\n=== SORTED BY EXPIRY (SOONEST FIRST) ===\n");
    bubble_sort_expiry(sorted, product_count);

    printf("%-4s | %-25s | %8s | %5s | %10s\n", "PRODUCT ID", "PRODUCT NAME", "UNIT PRICE", "QUANTITY", "EXPIRY DATE");
    printf("---------------------------------------------------------------------------------------\n");

    for (int i = 0; i < product_count; i++) {
        printf("%-4d | %-24.24s | P%7.2f | %4d | %-9s\n",
               sorted[i].id, sorted[i].name, sorted[i].price,
               sorted[i].quantity, sorted[i].expiration);
    }
    printf("---------------------------------------------------------------------------------------\n");
}

static void view_products_price_sorted(void) {
    if (product_count == 0) return;

    Product sorted[MAX_PRODUCTS];
    memcpy(sorted, inventory, product_count * sizeof(Product));

    printf("\n=== SORTED BY PRICE (LOWEST FIRST) ===\n");
    insertion_sort_price(sorted, product_count);

    printf("%-4s | %-25s | %8s | %5s | %10s\n", "PRODUCT ID", "PRODUCT NAME", "UNIT PRICE", "QUANTITY", "EXPIRY DATE");
    printf("---------------------------------------------------------------------------------------\n");

    for (int i = 0; i < product_count; i++) {
        printf("%-4d | %-24.24s | P%7.2f | %4d | %-9s\n",
               sorted[i].id, sorted[i].name, sorted[i].price,
               sorted[i].quantity, sorted[i].expiration);
    }
    printf("---------------------------------------------------------------------------------------\n");
}

static void search_product(void) {
    if (product_count == 0) {
        printf("\nNo products added!\n");
        return;
    }

    printf("\n=== SEARCH PRODUCT ===\n");
    printf("Choose between search by [1]Product ID [2]Product Name\n");
    int choice = get_valid_int("Enter your choice: ", 1, 2);

    if (choice == 1) {
        int id = get_valid_int("ENTER Product ID: ", 1, 9999);
        int index = linear_search_id(inventory, product_count, id);
        if (index != -1) {
            printf("\nFOUND: %s (Product ID:%d) P%.2f | Quantity:%d | %s\n",
                   inventory[index].name, inventory[index].id,
                   inventory[index].price, inventory[index].quantity,
                   inventory[index].expiration);
        } else printf("NO PRODUCT FOUND!\n");
    } else {
        char name[50]; //--max_name_length
        printf("Enter Product Name to search: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';
        int index = linear_search_name(inventory, product_count, name);
        if (index != -1) {
            printf("\nFOUND: %s (Product ID:%d) P%.2f | Quantity:%d | %s\n",
                   inventory[index].name, inventory[index].id,
                   inventory[index].price, inventory[index].quantity,
                   inventory[index].expiration);
        } else printf("NO PRODUCT FOUND!\n");
    }
}

static void update_product(void) {
    if (product_count == 0) {
        printf("\nNo products added.\n");
        return;
    }

    printf("\n=== UPDATE PRODUCT ===\n");
    int id = get_valid_int("Enter Product ID: ", 1, 9999);
    int index = find_product(id);

    if (index == -1) {
        printf("ID ENTERED %d NOT FOUND!\n", id);
        return;
    }

    printf("Current: %s (Product ID: %d)\n", inventory[index].name, id);
    printf("Choose your change transaction: \n");
    
    //-- UPDDATE TRANSACTIONS == [1]Unit Price  [2]Quantity  [3]Expiry Date  [4]Product Name  [5]Change All
    //-- A user have a choice if he/she wants only to update the Name or Price or Quantity or Expiry Date only but still have a choice to change all the details of inventory
    printf("[1]Product Name  [2]Unit Price  [3]Quantity  [4]Expiry Date  [5]Change All\n");
    int choice = get_valid_int("Enter your Choice (1-5): ", 1, 5);
    switch (choice) {
    case 1:
        get_valid_product_name("Enter New Product Name: ", inventory[index].name, MAX_NAME_LENGTH);
        break;
    case 2:
    	inventory[index].price = get_valid_float("Enter New Price: ", 0.01);
        break;
    case 3:
    	inventory[index].quantity = get_valid_int("Enter New Quantity: ", 0, 99999);
        break;
    case 4:
    	get_valid_expiry_date("Enter New Expiry date: ", inventory[index].expiration, MAX_DATE_LENGTH);
        break;
    case 5:
        get_valid_product_name("Enter New Product Name: ", inventory[index].name, MAX_NAME_LENGTH);
        inventory[index].price = get_valid_float("Enter New Price: ", 0.01);
        inventory[index].quantity = get_valid_int("Enter New Quantity: ", 0, 99999);
        get_valid_expiry_date("Enter New Expiry Date: ", inventory[index].expiration, MAX_DATE_LENGTH);
        break;
    }
    printf("\nProduct Updated Successfully!\n");
}

static void delete_product(void) {
    if (product_count == 0) {
        printf("\nNo products added!\n");
        return;
    }

    printf("\n=== DELETE PRODUCT ===\n");
    int id = get_valid_int("Enter Product ID: ", 1, 9999);
    int index = find_product(id);

    if (index == -1) {
        printf("ID [%d] NOT FOUND!\n", id);
        return;
    }

    printf("Are you sure you want to delete ['%s']? ENTER [1 = YES] [0 = NO]: ", inventory[index].name );
    if (get_valid_int("", 0, 1)) {
        for (int i = index; i < product_count - 1; i++) {
            inventory[i] = inventory[i + 1];
        }
        product_count--;
        printf("\nProduct Deleted Successfully!\n");
    } else {
        printf("\nDeletion Cancelled!\n");
    }
}

// ==================== MAIN 
static int show_menu(void) {
    printf("+----------------------------------------------------------------+\n");
    printf("|          WELCOME TO MAYAMART INVENTORY MANAGEMENT SYSTEM       |\n");
    printf("| Choose your transaction:                                       |\n");
    printf("| 1.Add Product      		5.Sort Product by Price          |\n");
    printf("| 2.View Inventory   		6.Sort Product by Expiry         |\n");
    printf("| 3.Search Product   		7.Delete Product                 |\n");
    printf("| 4.Update Product       	8.EXIT                           |\n");
    printf("+----------------------------------------------------------------+\n");
    printf("Enter your Tramsaction Choice [1-9]: ");
    return get_valid_int("", 1, 9);
}

int main(void) {
    printf("Mayamart Inventory Management System Started!\n");
    printf("Note in entering your inventories:\n");
    printf("For == Product Name [allow only letters - no NUMBERS ALLOWED!]\n");
    printf("For == Expiry Date [date must be TODAY or FUTURE!]\n\n");

    int choice;
    do {
        choice = show_menu();
        switch (choice) {
        case 1:
            add_product();
            break;
        case 2:
            view_products();
            break;
        case 3:
            search_product();
            break;
        case 4:
            update_product();
            break;
        case 5:
        	view_products_price_sorted();
            break;
        case 6:
            view_products_expiry_sorted();
            break;
        case 7:
            delete_product();
            break;
        case 8:
            printf("\nThank you for using Mayamart Inventory Management System!");
            printf("\n== to secure and store your Shop Inventory ==\n");
            printf("\n Programmed by Ms. Stephanie Clacio and Ms. Ma.Princess Marticio\n");
            break;
        }
        if (choice != 9) {
            printf("\n[Press Enter to continue...]");
            clear_input_buffer();
            getchar();
        }
    } while (choice != 9);

    return 0;
}
