#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>      // For rand() and srand() functions
#include <ctime>        // For time() function to seed srand()
#include <sstream>      // For stringstream operations
#include <algorithm>    // For algorithms like std::swap
#include <unordered_map> // For unordered_map container

const int SIZE = 1000;  // Constant size for array and file operations

// Function declarations
void createBinaryFile(const std::string& name, int length);
void writeBinary(int* values, int length, const std::string& name);
void selection_sort(int* values, int size);
bool binary_search_recursive(int* values, int key, int start, int end);
bool binary_search(int* values, int size, int key);

// Base class for data analysis
class Analyzer {
protected:
    int* values;    // Pointer to array of values
    int size;       // Size of the array

    // Helper function to copy values into internal array
    void cloneValues(int* values, int size) {
        this->size = size;
        this->values = new int[size];
        std::copy(values, values + size, this->values);
    }

public:
    // Constructor initializes values from input array
    Analyzer(int* values, int size) : values(nullptr), size(0) {
        cloneValues(values, size);
    }

    // Virtual destructor to ensure proper cleanup
    virtual ~Analyzer() {
        delete[] values;
    }

    // Virtual function to be overridden by derived classes
    virtual std::string analyze() = 0;
};

// Derived class for statistical analysis
class StatisticsAnalyzer : public Analyzer {
public:
    // Constructor sorts values and initializes base class
    StatisticsAnalyzer(int* values, int size) : Analyzer(values, size) {
        selection_sort(this->values, this->size);
    }

    // Analyze method computes statistical measures
    std::string analyze() override {
        if (size == 0) return "No data to analyze.";  // Handle empty array case

        int min = values[0];    // Initialize min value
        int max = values[size - 1]; // Initialize max value
        double sum = 0; // Initialize sum for mean calculation

        // Map to store frequency of each value
        std::unordered_map<int, int> frequencyMap;
        for (int i = 0; i < size; ++i) {
            sum += values[i];
            frequencyMap[values[i]]++;
        }

        double mean = sum / size;   // Calculate mean

        // Calculate median
        double median = (size % 2 == 0) ? (values[size / 2 - 1] + values[size / 2]) / 2.0 : values[size / 2];

        // Calculate mode
        int mode = values[0];
        int maxFrequency = 1;
        for (const auto& pair : frequencyMap) {
            if (pair.second > maxFrequency) {
                maxFrequency = pair.second;
                mode = pair.first;
            }
        }

        // Prepare result string
        std::ostringstream result;
        result << "The minimum value is " << min << "\n";
        result << "The maximum value is " << max << "\n";
        result << "The mean value is " << mean << "\n";
        result << "The median value is " << median << "\n";
        result << "The mode value is " << mode << " which occurred " << maxFrequency << " times";

        return result.str();    // Return result as string
    }
};

// Derived class for detecting duplicated values
class DuplicateAnalyzer : public Analyzer {
public:
    // Constructor initializes base class
    DuplicateAnalyzer(int* values, int size) : Analyzer(values, size) {}

    // Analyze method counts duplicated values
    std::string analyze() override {
        std::unordered_map<int, int> countMap;
        for (int i = 0; i < size; ++i) {
            countMap[values[i]]++;
        }

        int duplicateCount = 0;
        for (const auto& pair : countMap) {
            if (pair.second > 1) {
                duplicateCount += pair.second - 1;
            }
        }

        std::ostringstream result;
        result << "There were " << duplicateCount << " duplicated values";
        return result.str();
    }
};

// Derived class for detecting missing values
class MissingAnalyzer : public Analyzer {
public:
    // Constructor initializes base class
    MissingAnalyzer(int* values, int size) : Analyzer(values, size) {}

    // Analyze method counts missing values
    std::string analyze() override {
        std::unordered_map<int, bool> valueMap;
        for (int i = 0; i < size; ++i) {
            valueMap[values[i]] = true;
        }

        int missingCount = 0;
        for (int i = 0; i < 1000; ++i) {
            if (valueMap.find(i) == valueMap.end()) {
                missingCount++;
            }
        }

        std::ostringstream result;
        result << "There were " << missingCount << " missing values";
        return result.str();
    }
};

// Derived class for searching random values
class SearchAnalyzer : public Analyzer {
public:
    // Constructor sorts values and initializes base class
    SearchAnalyzer(int* values, int size) : Analyzer(values, size) {
        selection_sort(this->values, this->size);
    }

    // Analyze method searches for random values
    std::string analyze() override {
        int foundCount = 0;
        for (int i = 0; i < 100; ++i) {
            int searchValue = rand() % 1000;
            if (binary_search(values, size, searchValue)) {
                foundCount++;
            }
        }

        std::ostringstream result;
        result << "There were " << foundCount << " random values found";
        return result.str();
    }
};

// Class for reading binary data from file
class BinaryReader {
    int* values;    // Pointer to array of values
    int size;       // Size of the array

    // Helper function to read values from binary file
    void readValues(const std::string& name) {
        std::ifstream inFile(name, std::ios::binary);   // Open file in binary mode
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));   // Read size of array
        values = new int[size]; // Allocate memory for array
        inFile.read(reinterpret_cast<char*>(values), size * sizeof(int));  // Read array elements
        inFile.close(); // Close file stream
    }

public:
    // Constructor reads values from binary file
    BinaryReader(const std::string& name) : values(nullptr), size(0) {
        readValues(name);
    }

    // Destructor frees allocated memory
    ~BinaryReader() {
        delete[] values;
    }

    // Getter for values pointer
    int* getValues() const {
        return values;
    }

    // Getter for size of array
    int getSize() const {
        return size;
    }
};

// Function to create a binary file with random data
void createBinaryFile(const std::string& name, int length) {
    std::vector<int> array(length); // Create vector to hold random values
    for (int& num : array) {
        num = rand() % 1000;    // Generate random number between 0 and 999
    }
    writeBinary(array.data(), length, name);   // Write vector data to binary file
}

// Function to write data into a binary file
void writeBinary(int* values, int length, const std::string& name) {
    std::ofstream outFile(name, std::ios::binary); // Open file in binary mode
    outFile.write(reinterpret_cast<const char*>(&length), sizeof(length));   // Write size of array
    outFile.write(reinterpret_cast<const char*>(values), length * sizeof(int)); // Write array elements
    outFile.close();    // Close file stream
}

// Function to perform selection sort on array
void selection_sort(int* values, int size) {
    for (int i = 0; i < size - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < size; ++j) {
            if (values[j] < values[minIndex]) {
                minIndex = j;
            }
        }
        std::swap(values[i], values[minIndex]);    // Swap elements to sort
    }
}

// Recursive function for binary search
bool binary_search_recursive(int* values, int key, int start, int end) {
    if (start > end) {
        return false;
    }
    int mid = start + (end - start) / 2;
    if (values[mid] == key) {
        return true;
    }
    if (values[mid] > key) {
        return binary_search_recursive(values, key, start, mid - 1);
    }
    return binary_search_recursive(values, key, mid + 1, end);
}

// Function to perform binary search on array
bool binary_search(int* values, int size, int key) {
    return binary_search_recursive(values, key, 0, size - 1);
}

// Main function
int main() {
    // Program introduction
    std::cout << "Binary Data Analyzer\n" << "\n";

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    // Create the binary file
    createBinaryFile("binary.dat", SIZE);

    // Create a BinaryReader instance
    BinaryReader br("binary.dat");

    // Create an instance of the StatisticsAnalyzer class
    StatisticsAnalyzer sa(br.getValues(), br.getSize());
    std::cout << sa.analyze() << '\n';

    // Create an instance of the DuplicateAnalyzer class
    DuplicateAnalyzer da(br.getValues(), br.getSize());
    std::cout << da.analyze() << '\n';

    // Create an instance of the MissingAnalyzer class
    MissingAnalyzer ma(br.getValues(), br.getSize());
    std::cout << ma.analyze() << '\n';

    // Create an instance of the SearchAnalyzer class
    SearchAnalyzer ra(br.getValues(), br.getSize());
    std::cout << ra.analyze() << '\n';

    return 0;
}