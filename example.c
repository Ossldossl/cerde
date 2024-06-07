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
}