#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "Bounded_Buffer.h"
#include "Producer.h"
using namespace std;

// Constructor for Producer
Producer::Producer(int id, int numProducts, Bounded_Buffer *buffer)
{
    this->id = id;
    this->numProducts = numProducts;
    this->buffer = buffer;
    this->types_ = {"SPORTS", "NEWS", "WEATHER"};
    this->types_cnt = {{"SPORTS", 0}, {"NEWS", 0}, {"WEATHER", 0}};
}

// Function to produce products and insert them into the buffer
void Producer::produce()
{
    for (int i = 0; i < numProducts; i++)
    {
        string type = types_[rand() % 3]; // Randomly select a product type
        string product = "producer "+to_string(id) +" "+ type +" "+ to_string(types_cnt[type]); // Create the product string
        types_cnt[type]++; // Increment the count for the selected product type
        char *product_c = new char[product.size() + 1]; // Allocate memory for the product string
        strcpy(product_c, product.c_str()); // Copy the product string to the allocated memory
        buffer->insert(product_c); // Insert the product into the buffer
    }
    char done[]= "DONE";
    buffer->insert(done); // Insert a "DONE" message to indicate production is complete
    return;
}