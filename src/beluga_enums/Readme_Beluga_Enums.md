This is based on a really clever piece of code from https://www.codeproject.com/Articles/32000/Improving-C-Enums-Adding-Serialization-Inheritance

C++ enums are integers. An enum cannot be inherited from without defining it as a class (which defeats the purpose of using an enum which is that it's small and simple).

This code uses powerful sorcery to create extensible enums, and also throws in useful features:
- iteration over enums
- conversion between string and enum


