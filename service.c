#include <stdio.h>
#include <Windows.h>

DWORD service_creation(LPCWSTR serviceName, LPCWSTR binaryPath) {
    DWORD SCManagerAccess = SC_MANAGER_ALL_ACCESS;

    SC_HANDLE hService;
    SC_HANDLE hSCManager;

    DWORD lastError;

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) {
        return GetLastError();
    }

    hService = CreateService(
        hSCManager,
        serviceName,
        serviceName,
        SC_MANAGER_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_NORMAL,
        binaryPath,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );

    if (hService == NULL) {
        lastError = GetLastError();
        if (lastError == ERROR_SERVICE_EXISTS) {
            printf("Service Already Exists...!\n");
        }
        else {
            printf("Weird Error : %d\n", lastError);
        }
        CloseServiceHandle(hSCManager); // Close SCManager handle before returning
        return lastError;
    }

    SERVICE_LAUNCH_PROTECTED_INFO info;
    info.dwLaunchProtected = SERVICE_LAUNCH_PROTECTED_ANTIMALWARE_LIGHT;
    if (ChangeServiceConfig2(hService, SERVICE_CONFIG_LAUNCH_PROTECTED, &info) == FALSE) {
        return GetLastError();
    }

    // Close the service and manager handles
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return 0; // Success
}

DWORD service_deletion(LPCWSTR serviceName) {
    SC_HANDLE hService;
    SC_HANDLE hSCManager;

    DWORD lastError;

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) {
        return GetLastError();
    }

    // Open the service using the provided service name
    hService = OpenService(hSCManager, serviceName, DELETE);
    if (hService == NULL) {
        lastError = GetLastError();
        if (lastError == ERROR_SERVICE_DOES_NOT_EXIST) {
            printf("Service does not exist.\n");
        }
        else {
            printf("Failed to open service, error code: %d\n", lastError);
        }
        CloseServiceHandle(hSCManager);
        return lastError;
    }

    // Delete the service
    if (!DeleteService(hService)) {
        lastError = GetLastError();
        printf("Failed to delete service, error code: %d\n", lastError);
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return lastError;
    }

    printf("Service deleted successfully.\n");

    // Close the handles
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return 0; // Success
}

DWORD service_start(LPCWSTR serviceName) {
    SC_HANDLE hService;
    SC_HANDLE hSCManager;

    DWORD lastError;

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) {
        return GetLastError();
    }

    // Open the service using the provided service name
    hService = OpenService(hSCManager, serviceName, SERVICE_START);
    if (hService == NULL) {
        lastError = GetLastError();
        if (lastError == ERROR_SERVICE_DOES_NOT_EXIST) {
            printf("Service does not exist.\n");
        }
        else {
            printf("Failed to open service, error code: %d\n", lastError);
        }
        CloseServiceHandle(hSCManager);
        return lastError;
    }

    // Start the service
    if (!StartService(hService, 0, NULL)) {
        lastError = GetLastError();
        printf("Failed to start service, error code: %d\n", lastError);
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return lastError;
    }

    printf("Service started successfully.\n");

    // Close the handles
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return 0; // Success
}

DWORD service_stop(LPCWSTR serviceName) {
    SC_HANDLE hService;
    SC_HANDLE hSCManager;
    SERVICE_STATUS serviceStatus;

    DWORD lastError;

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) {
        return GetLastError();
    }

    // Open the service using the provided service name
    hService = OpenService(hSCManager, serviceName, SERVICE_STOP);
    if (hService == NULL) {
        lastError = GetLastError();
        if (lastError == ERROR_SERVICE_DOES_NOT_EXIST) {
            printf("Service does not exist.\n");
        }
        else {
            printf("Failed to open service, error code: %d\n", lastError);
        }
        CloseServiceHandle(hSCManager);
        return lastError;
    }

    // Stop the service
    if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
        lastError = GetLastError();
        printf("Failed to stop service, error code: %d\n", lastError);
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return lastError;
    }

    printf("Service stopped successfully.\n");

    // Close the handles
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return 0; // Success
}

int wmain(int argc, wchar_t** argv) {
    if (argc < 3) {
        wprintf(L"Usage: %s <create|delete|start|stop> <service_name> <path_to_binary>\n", argv[0]);
        return 1;
    }

    if (_wcsicmp(argv[1], L"create") == 0) {
        if (argc < 4) {
            wprintf(L"Usage for create: %s create <service_name> <path_to_binary>\n", argv[0]);
            return 1;
        }
        DWORD retval = service_creation(argv[2], argv[3]); // Pass the service name and binary path
        if (retval != 0) {
            printf("Failed to create service, error code: %d\n", retval);
            return retval;
        }
        printf("Service created successfully.\n");

        // Start the service after creation
        retval = service_start(argv[2]);
        if (retval != 0) {
            printf("Failed to start service, error code: %d\n", retval);
            return retval;
        }
    }
    else if (_wcsicmp(argv[1], L"delete") == 0) {
        DWORD retval = service_deletion(argv[2]); // Pass the service name for deletion
        if (retval != 0) {
            printf("Failed to delete service, error code: %d\n", retval);
            return retval;
        }
    }
    else if (_wcsicmp(argv[1], L"start") == 0) {
        DWORD retval = service_start(argv[2]); // Pass the service name to start it
        if (retval != 0) {
            printf("Failed to start service, error code: %d\n", retval);
            return retval;
        }
    }
    else if (_wcsicmp(argv[1], L"stop") == 0) {
        DWORD retval = service_stop(argv[2]); // Pass the service name to stop it
        if (retval != 0) {
            printf("Failed to stop service, error code: %d\n", retval);
            return retval;
        }
    }
    else {
        wprintf(L"Invalid option. Use 'create', 'delete', 'start', or 'stop'.\n");
        return 1;
    }

    return 0;
}
