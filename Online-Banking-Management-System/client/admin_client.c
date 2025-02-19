#include "common_client.c"

void display_admin_menu()
{
    printf("1️⃣ ➕ 👔 Add New Bank Employee\n");
    printf("2️⃣ 🛠️ ✏️ Modify Customer/Employee Details\n");
    printf("3️⃣ ⚙️ 👥 Manage User Roles\n");
    printf("4️⃣ 🔒 🔄 Change Password\n");
    printf("5️⃣ 🚪 👋 Logout\n");
}


void add_new_employee(int sock_fd, Token *user)
{
    Request req;
    Response res;

    char username[USERNAME_SIZE], password[PASSWORD_SIZE];

    printf("\nEnter the username of the new employee: ");
    scanf("%s", username);

    printf("\nEnter the password of the new employee: ");
    scanf("%s", password);

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "ADD_EMPLOYEE %s %s", username, password);
    req.argc = 3;
    req.user = *user;

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
    {
        printf("Could not send the request to the server\n");
        return;
    }

    if (read(sock_fd, &res, sizeof(Response)) < 0)
    {
        printf("Could not receive the response from the server\n");
        return;
    }

    printf("%s\n", res.body);
}

void modify_detalis(int sock_fd, Token *user, UserType user_type)
{
    Request req;
    Response res;
    int choice, user_id;
    char field[10], *value;

    req.user = *user;
    
    printf("\nEnter the ID of the user: ");
    scanf("%d", &user_id);
    printf("\n1. USERNAME\n");
    printf("2. PASSWORD\n");
    printf("\nEnter which detail would you like to modify: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        strcpy(field, "USERNAME");
        value = (char*) malloc(USERNAME_SIZE * sizeof(char));
    }
    else if (choice == 2)
    {
        strcpy(field, "PASSWORD");
        value = (char*) malloc(PASSWORD_SIZE * sizeof(char));
    }
    else
    {
        printf("\nInvalid choice. Please try again.\n");
        return;
    }

    printf("\nEnter the new updated value: ");
    scanf("%s", value);

    req.argc = 4;

    if (user_type == CUSTOMER)
        snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "MODIFY_CUSTOMER %d %s %s", user_id, field, value);
    else if (user_type == EMPLOYEE)
        snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "MODIFY_EMPLOYEE %d %s %s", user_id, field, value);
    else
        return;

    free(value);
    value = NULL;

    if (send(sock_fd, &req, sizeof(Request), 0) < 0) return;
    if (read(sock_fd, &res, sizeof(Response)) < 0) return;

    printf("\n%s\n", res.body);
}

void manage_user_roles(int sock_fd, Token* user)
{
    Request req;
    Response res;
    int employee_id;
    int role;

    req.argc = 3;
    req.user = *user;

    printf("Enter the Employee ID whose role is to be changed: ");
    scanf("%d", &employee_id);

    printf("Enter the new role of the given employee:\n");
    printf("1. Regular Employee\n");
    printf("2. Manager\n");
    printf("\nChoice: ");
    scanf("%d", &role);

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "CHANGE_USER_ROLE %d %d", employee_id, role);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0) return;
    if (read(sock_fd, &res, sizeof(Response)) < 0) return;

    printf("\n%s\n", res.body);
}

void change_admin_password(int sock_fd, Token* user)
{
    Request req;
    Response res;
    char new_pass[PASSWORD_SIZE];

    req.user = *user;
    req.argc = 2;

    printf("Enter the new password: ");
    scanf("%s", new_pass);

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "PASSWORD_CHANGE %s", new_pass);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0) return;
    if (read(sock_fd, &res, sizeof(Response)) < 0) return;

    printf("\n%s\n", res.body);
}

void admin_handler(int sock_fd, Token *user)
{
    while (1)
    {
        int choice;

        printf("👋 Hello, %s\n", user->username);  
        printf("🆔 Admin ID: %d\n", user->user_id);  
        display_admin_menu();

        printf("📥 ✏️ Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("🆕 Adding a new employee... 👔\n");
            add_new_employee(sock_fd, user);
            break;

        case 2:
            {
                UserType type;
                printf("\n1️⃣ 👤 CUSTOMER");
                printf("\n2️⃣ 🧑‍💼 Employee");
                printf("\n📝 Enter the type of user you would like to modify: ");
                scanf("%d", (int*)&type);
                printf("🛠️ Modifying details...\n");
                modify_detalis(sock_fd, user, type);
            }
            break;

        case 3:
            printf("⚙️ Managing user roles...\n");
            manage_user_roles(sock_fd, user);
            break;

        case 4:
            printf("🔒 Changing admin password...\n");
            change_admin_password(sock_fd, user);
            break;

        case 5:
            printf("🚪 Logging out... 👋\n");
            logout(sock_fd, user);
            return;  // Exit after logout

        default:
            printf("❌ Invalid choice! Please try again.\n");
            continue;
        }

        if (user->user_id == -1) return;  // Exit if user is de authenticated
    }
}
