#include <windows.h>
#include <string>
#include <sstream>
#include "AVL.h"

#define ID_INSERTION 1
#define ID_TRAVERSALS 2
#define ID_SEARCH 3
#define ID_DELETION 4
#define ID_EXIT 5
#define ID_INPUT 6

#define COLOR_BACKGROUND RGB(240, 240, 240)
#define COLOR_BUTTON RGB(200, 200, 200)
#define COLOR_TEXT RGB(0, 0, 0)

struct nodo* root = nullptr;

// Function prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void drawTree(HDC hdc, struct nodo* node, int x, int y, int xOffset);
void performTraversal(HWND hwnd, int traversalType);
void performSearch(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "AVL Tree GUI Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "AVL Tree Operations",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hInput;

    switch (uMsg) {
    case WM_CREATE:
        CreateWindow("BUTTON", "Insert", WS_VISIBLE | WS_CHILD, 10, 10, 100, 30, hwnd, (HMENU)ID_INSERTION, NULL, NULL);
        CreateWindow("BUTTON", "Traversals", WS_VISIBLE | WS_CHILD, 120, 10, 100, 30, hwnd, (HMENU)ID_TRAVERSALS, NULL, NULL);
        CreateWindow("BUTTON", "Search", WS_VISIBLE | WS_CHILD, 230, 10, 100, 30, hwnd, (HMENU)ID_SEARCH, NULL, NULL);
        CreateWindow("BUTTON", "Delete", WS_VISIBLE | WS_CHILD, 340, 10, 100, 30, hwnd, (HMENU)ID_DELETION, NULL, NULL);
        CreateWindow("BUTTON", "Exit", WS_VISIBLE | WS_CHILD, 450, 10, 100, 30, hwnd, (HMENU)ID_EXIT, NULL, NULL);
        hInput = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 50, 100, 25, hwnd, (HMENU)ID_INPUT, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_INSERTION: {
            char buffer[256];
            GetWindowText(hInput, buffer, 256);
            int value = atoi(buffer);
            if (value != 0) {
                root = insertar(root, value);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
        case ID_TRAVERSALS:
            performTraversal(hwnd, 0);  // 0 for in-order, you can modify this
            break;
        case ID_SEARCH:
            performSearch(hwnd);
            break;
        case ID_DELETION: {
            char buffer[256];
            GetWindowText(hInput, buffer, 256);
            int value = atoi(buffer);
            if (value != 0) {
                root = eliminar(root, value);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
        case ID_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        if (root != nullptr) {
            drawTree(hdc, root, 400, 100, 200);
        }
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void drawTree(HDC hdc, struct nodo* node, int x, int y, int xOffset) {
    if (node == nullptr) return;

    // Draw the current node
    Ellipse(hdc, x - 20, y - 20, x + 20, y + 20);
    
    // Draw the node's value
    std::string value = std::to_string(node->clave);
    TextOut(hdc, x - 10, y - 10, value.c_str(), value.length());

    // Draw lines to children
    if (node->izquierda) {
        MoveToEx(hdc, x, y + 20, NULL);
        LineTo(hdc, x - xOffset, y + 80);
        drawTree(hdc, node->izquierda, x - xOffset, y + 100, xOffset / 2);
    }
    if (node->derecha) {
        MoveToEx(hdc, x, y + 20, NULL);
        LineTo(hdc, x + xOffset, y + 80);
        drawTree(hdc, node->derecha, x + xOffset, y + 100, xOffset / 2);
    }
}

void performTraversal(HWND hwnd, int traversalType) {
    std::stringstream ss;
    switch (traversalType) {
    case 0: // In-order
        ss << "In-order traversal: ";
        inOrden(root);
        break;
    case 1: // Pre-order
        ss << "Pre-order traversal: ";
        preOrden(root);
        break;
    case 2: // Post-order
        ss << "Post-order traversal: ";
        postOrden(root);
        break;
    }
    MessageBox(hwnd, ss.str().c_str(), "Traversal Result", MB_OK);
}

void performSearch(HWND hwnd) {
    char buffer[256];
    GetWindowText(GetDlgItem(hwnd, ID_INPUT), buffer, 256);
    int value = atoi(buffer);
    if (value != 0) {
        ResultadoBusqueda result = bfs(root, value);
        std::stringstream ss;
        if (result.encontrado) {
            ss << "Value " << value << " found at level " << result.nivel
               << "\nPosition: " << result.posicion;
        } else {
            ss << "Value " << value << " not found in the tree.";
        }
        MessageBox(hwnd, ss.str().c_str(), "Search Result", MB_OK);
    }
}
