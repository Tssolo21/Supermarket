#include <iostream>

// Declarations of test suite entry points
int test_models_main();
int test_dal_main();
int test_services_main();
int test_observer_main();

int main() {
    std::cout << "Starting Supermarket Hero System Test Suite..." << std::endl;
    
    int result = 0;
    
    try {
        std::cout << "Running Model Tests..." << std::endl;
        result |= test_models_main();
        
        std::cout << "Running DAL Tests..." << std::endl;
        result |= test_dal_main();
        
        std::cout << "Running Service Tests..." << std::endl;
        result |= test_services_main();
        
        std::cout << "Running Observer Integration Tests..." << std::endl;
        result |= test_observer_main();
    } catch (const std::exception& e) {
        std::cerr << "\nFATAL: Uncaught exception during testing: " << e.what() << std::endl;
        return 1;
    }
    
    if (result == 0) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "       ALL TEST SUITES PASSED!         " << std::endl;
        std::cout << "========================================" << std::endl;
    } else {
        std::cout << "\n========================================" << std::endl;
        std::cout << "       SOME TEST SUITES FAILED!        " << std::endl;
        std::cout << "========================================" << std::endl;
    }
    
    return result;
}
