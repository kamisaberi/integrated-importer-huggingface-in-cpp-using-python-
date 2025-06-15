#include <torch/script.h>
#include <torch/torch.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cstdlib> // For std::system
#include <fstream> // For checking if file exists

bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

int main() {
    const std::string model_filename = "resnet18_traced.pt";
    const std::string python_script_name = "convert_model.py";

    // --- THIS IS THE KEY CHANGE ---
    // Define the path to the python executable inside our virtual environment
    const std::string python_executable = "./cpp_venv/bin/python3";

    std::cout << "--- C++ Orchestrator for Model Conversion and Inference ---" << std::endl;

    if (!file_exists(model_filename)) {
        std::cout << "\nModel file '" << model_filename << "' not found." << std::endl;
        std::cout << "Attempting to create it by running the Python conversion script..." << std::endl;

        // Construct the command to run the Python script using the venv python
        std::string command = python_executable; // Use the specific python
        command += " ";
        command += python_script_name;
        command += " ";
        command += model_filename;

        std::cout << "Executing command: " << command << std::endl;
        int return_code = std::system(command.c_str());

        if (return_code != 0) {
            std::cerr << "FATAL: Python script execution failed." << std::endl;
            return 1;
        }
        if (!file_exists(model_filename)) {
            std::cerr << "FATAL: Python script ran but did not create the model file." << std::endl;
            return 1;
        }
        std::cout << "Python script finished successfully." << std::endl;
    } else {
        std::cout << "Model file '" << model_filename << "' already exists. Skipping Python conversion." << std::endl;
    }

    // ... The rest of the file (loading and inference) is exactly the same ...
    torch::jit::script::Module model;
    try {
        std::cout << "\n--- C++ Inference ---" << std::endl;
        model = torch::jit::load(model_filename);
        model.eval();
        std::cout << "Model loaded successfully." << std::endl;
    } catch (const c10::Error& e) {
        std::cerr << "Error loading the model:\n" << e.what() << std::endl;
        return -1;
    }

    auto input_tensor = torch::randn({1, 3, 224, 224});
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(input_tensor);

    torch::NoGradGuard no_grad;
    at::Tensor output = model.forward(inputs).toTensor();
    auto prediction = output.argmax(1);

    std::cout << "\n--- Results ---" << std::endl;
    std::cout << "Inference successful." << std::endl;
    std::cout << "Output shape: " << output.sizes() << std::endl;
    std::cout << "Predicted Class ID: " << prediction.item<int64_t>() << std::endl;

    return 0;
}