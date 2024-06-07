# cerde
serialize and deserialize data from/to json in C

# Usage
To use cerde, simple add cerde.c to your build system and include the cerde.h header.

## Example
```c
int main()
{
    cd_init();

    // create the root object
    cd_val* root = cd_add_obj(null, make_sstr(""));
    // now add children to it
        cd_add(root, make_sstr("test_number"), 12.5678f);
        cd_add(root, make_sstr("test_bool"), true);
        // add an array as a child of the root
        cd_val* temperatures_array = cd_add_array(root, make_sstr("temperatures"));
            // make_sstr is a macro which expands to string + length of string. This macro only works for string literals
            cd_add_element(temperatures_array, 15);
            cd_add_element(temperatures_array, 16);
            cd_add_element(temperatures_array, 17);
            cd_add_element(temperatures_array, 18);
            cd_add_element(temperatures_array, 19);
            cd_add_element(temperatures_array, 21);
            cd_add_element(temperatures_array, 22);
            cd_add_element(temperatures_array, 23);
            cd_add_element(temperatures_array, 24);

    // when you're finished creating your object, serialize it to a string!
    str result = cd_serialize(root);
    printf("%s", result.data); // outputs: "{"test_number":12.5678,"test_bool":1,"temperatures":[15,16,17,18,19,21,22,23,24]}"
    // finally, deinitialize cerde
    cd_destroy();
}
```