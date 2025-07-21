// UseDLLTest.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <Windows.h>

int main()
{
    std::cout << "=== Test avec chargement dynamique ===" << std::endl;

    // Charger la DLL
    HMODULE hDLL = LoadLibrary(L"TronEngine.dll");

    if (hDLL == NULL) {
        std::cout << "Erreur : Impossible de charger MathDLL.dll" << std::endl;
        std::cout << "Assurez-vous que la DLL est dans le même dossier que l'exe" << std::endl;
        system("pause");
        return -1;
    }

    std::cout << "DLL chargée avec succès!" << std::endl;
    typedef int (*AddFunc)(int, int);
    typedef int (*MultFunc)(int, int);
    typedef float (*DivFunc)(float, float);
    typedef const char* (*GetMsgFunc)();

    AddFunc pAdd = (AddFunc)GetProcAddress(hDLL, "Addition");
    MultFunc pMult = (MultFunc)GetProcAddress(hDLL, "Multiplication");
    DivFunc pDiv = (DivFunc)GetProcAddress(hDLL, "Division");
    GetMsgFunc pGetMsg = (GetMsgFunc)GetProcAddress(hDLL, "GetDLLMessage");

    if (pAdd && pMult && pDiv && pGetMsg) {
        std::cout << "\nToutes les fonctions trouvées!" << std::endl;

        // Utiliser les fonctions
        std::cout << "\nMessage: " << pGetMsg() << std::endl;
        std::cout << "5 + 3 = " << pAdd(5, 3) << std::endl;
        std::cout << "4 * 7 = " << pMult(4, 7) << std::endl;
        std::cout << "10 / 3 = " << pDiv(10.0f, 3.0f) << std::endl;
    }
    else {
        std::cout << "Erreur : Impossible de trouver toutes les fonctions" << std::endl;
    }

    FreeLibrary(hDLL);
    std::cout << "\nDLL liberee" << std::endl;

    system("pause");
    return 0;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
