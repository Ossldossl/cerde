#include <stdio.h>
#include "cerde.h"

int main() 
{
    cd_init();
    
    cd_val* enemy = cd_add_obj(null, make_sstr(""));
        cd_add(enemy, make_sstr("name"), make_sstr("Wizard"));
        cd_add(enemy, make_sstr("damage"), 12.5f);
        cd_add(enemy, make_sstr("move_distance"), 3);
    
    cd_val* enemy_abilities = cd_add_array(enemy, make_sstr("abilities")); // length is optional
        cd_val* first_ability = cd_add_obj(enemy_abilities, make_sstr(""));
            cd_add(first_ability, make_sstr("name"), make_sstr("poisoning"));
            cd_add(first_ability, make_sstr("damage"), 7.5f);
            cd_add(first_ability, make_sstr("duration"), 20.f);
        cd_val* second_ability = cd_add_obj(enemy_abilities, make_sstr(""));
            cd_add(second_ability, make_sstr("name"), make_sstr("punch"));
            cd_add(second_ability, make_sstr("damage"), 16.f);
            cd_add(second_ability, make_sstr("duration"), 0.f);
    
    str result = cd_serialize(enemy);
    printf("%s", result.data);

    cd_reset();
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

    result = cd_serialize(root);
    printf("\n---\n%s", result.data);

    cd_destroy();
}