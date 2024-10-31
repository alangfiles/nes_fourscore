/*	example code for cc65, for NES
 *  use the four score to move 4 players,
 *  around a collideable bg
 *	using neslib, nesdoug, and nesfourscore
 *	Based off code by Doug Fraker 2018 (www.nesdoug.com)
 *  Written by Alan Files 2024
 */	
 

#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "LIB/nesfourscore.h"  
#include "Sprites.h" // holds our metasprite data
#include "fourscore.h"
	
	
void main (void) {
	
	ppu_off(); // screen off

	clear_vram_buffer();
	
	// load the palettes
	pal_bg(palette_bg);
	pal_spr(palette_sp);

	// use the second set of tiles for sprites
	// both bg and sprites are set to 0 by default
	bank_spr(1);

	set_vram_buffer(); // do at least once, sets a pointer to a buffer

	load_room();
	
	set_scroll_y(0xff); // shift the bg down 1 pixel
	
	ppu_on_all(); // turn on screen

	
	while (1){
		// infinite loop
		ppu_wait_nmi(); // wait till beginning of the frame
		
		//read controllers 1 and 3 into an integer
		doublepad = pad_poll_4score_1_3();
		pad1 = high_byte(doublepad); //the high byte is the first controller's input
		pad3 = low_byte(doublepad); // low byte is the third controller's input

		//read controllers 2 and 4 into an integer
		doublepad = pad_poll_4score_2_4();
		pad2 = high_byte(doublepad);
		pad4 = low_byte(doublepad);

		// Deal with movement for each player
		
		//setup generics
		GenericBoxGuy = BoxGuy1;
		generic_pad = pad1;
		//call movement for generics
		movement();
		//reassign boxguy1's position based off generic's result
		BoxGuy1.x = GenericBoxGuy.x;
		BoxGuy1.y = GenericBoxGuy.y;

		//do the same for the other 3 players
		GenericBoxGuy = BoxGuy2;
		generic_pad = pad2;
		movement();
		BoxGuy2.x = GenericBoxGuy.x;
		BoxGuy2.y = GenericBoxGuy.y;

		GenericBoxGuy = BoxGuy3;
		generic_pad = pad3;
		movement();
		BoxGuy3.x = GenericBoxGuy.x;
		BoxGuy3.y = GenericBoxGuy.y;


		GenericBoxGuy = BoxGuy4;
		generic_pad = pad4;
		movement();
		BoxGuy4.x = GenericBoxGuy.x;
		BoxGuy4.y = GenericBoxGuy.y;

		draw_sprites();
	}
}




void load_room(void){
	set_data_pointer(Room1);
	set_mt_pointer(metatiles1);
	for(y=0; ;y+=0x20){
		for(x=0; ;x+=0x20){
			address = get_ppu_addr(0, x, y);
			index = (y & 0xf0) + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			flush_vram_update2();
			if (x == 0xe0) break;
		}
		if (y == 0xe0) break;
	}
	
	set_vram_update(NULL); // just turn ppu updates OFF for this example
	
	//copy the room to the collision map
	memcpy (c_map, Room1, 240);
	
}




void draw_sprites(void){
	// clear all sprites from sprite buffer
	oam_clear();
	
	temp_x = BoxGuy1.x >> 8;
	temp_y = BoxGuy1.y >> 8;
	if(temp_x == 0) temp_x = 1;
	if(temp_y == 0) temp_y = 1;
	
	// draw 1 metasprite
	oam_meta_spr(temp_x, temp_y, RoundSpr1);

	temp_x = BoxGuy2.x >> 8;
	temp_y = BoxGuy2.y >> 8;
	if(temp_x == 0) temp_x = 1;
	if(temp_y == 0) temp_y = 1;
	
	// draw 1 metasprite
	oam_meta_spr(temp_x, temp_y, RoundSpr2);

	temp_x = BoxGuy3.x >> 8;
	temp_y = BoxGuy3.y >> 8;
	if(temp_x == 0) temp_x = 1;
	if(temp_y == 0) temp_y = 1;
	
	// draw 1 metasprite
	oam_meta_spr(temp_x, temp_y, RoundSpr3);

	temp_x = BoxGuy4.x >> 8;
	temp_y = BoxGuy4.y >> 8;
	if(temp_x == 0) temp_x = 1;
	if(temp_y == 0) temp_y = 1;
	
	// draw 1 metasprite
	oam_meta_spr(temp_x, temp_y, RoundSpr4);
}
	

	
	
void movement(void){
	// handle x
	old_x = GenericBoxGuy.x;
	
	if(generic_pad & PAD_LEFT){
		direction = LEFT;
		hero_velocity_x = -SPEED;
	}
	else if (generic_pad & PAD_RIGHT){
		direction = RIGHT;
		hero_velocity_x = SPEED;
	}
	else { // nothing pressed
		hero_velocity_x = 0;
	}
	
	GenericBoxGuy.x += hero_velocity_x;
	
	if(GenericBoxGuy.x > 0xf100) { // too far, don't wrap around
        
        if(old_x >= 0x8000){
            GenericBoxGuy.x = 0xf100; // max right
        }
        else{
            GenericBoxGuy.x = 0x0000; // max left
        }
        
	} 
	
	
	Generic.x = GenericBoxGuy.x >> 8; // the collision routine needs an 8 bit value
	Generic.y = GenericBoxGuy.y >> 8;
	Generic.width = HERO_WIDTH;
	Generic.height = HERO_HEIGHT;
	
	if(hero_velocity_x < 0){ // going left
		if(bg_coll_L() ){ // check collision left
            high_byte(GenericBoxGuy.x) = high_byte(GenericBoxGuy.x) - eject_L;
            
        }
	}
	else if(hero_velocity_x > 0){ // going right
		if(bg_coll_R() ){ // check collision right
            high_byte(GenericBoxGuy.x) = high_byte(GenericBoxGuy.x) - eject_R;
            
        }
	}
	// else 0, skip it
	
	
	
	// handle y
	old_y = GenericBoxGuy.y;

	if(generic_pad & PAD_UP){
		hero_velocity_y = -SPEED;
	}
	else if (generic_pad & PAD_DOWN){
		hero_velocity_y = SPEED;
	}
	else { // nothing pressed
		hero_velocity_y = 0;
	}

	GenericBoxGuy.y += hero_velocity_y;
	
	if(GenericBoxGuy.y > 0xe000) { // too far, don't wrap around
        
        if(old_y >= 0x8000){
            GenericBoxGuy.y = 0xe000; // max down
        }
        else{
            GenericBoxGuy.y = 0x0000; // max up
        }
        
	} 
	
	Generic.x = GenericBoxGuy.x >> 8; // the collision routine needs an 8 bit value
	Generic.y = GenericBoxGuy.y >> 8;
//	Generic.width = HERO_WIDTH; // already is this
//	Generic.height = HERO_HEIGHT;
	
	if(hero_velocity_y < 0){ // going up
		if(bg_coll_U() ){ // check collision left
            high_byte(GenericBoxGuy.y) = high_byte(GenericBoxGuy.y) - eject_U;
            
        }
	}
	else if(hero_velocity_y > 0){ // going down
		if(bg_coll_D() ){ // check collision right
            high_byte(GenericBoxGuy.y) = high_byte(GenericBoxGuy.y) - eject_D;
            
        }
	}
	// else 0, skip it
	
}	


	

char bg_coll_L(void){
    // check 2 points on the left side
    temp_x = Generic.x;
    
    eject_L = temp_x | 0xf0;
    temp_y = Generic.y + 2;
    if(bg_collision_sub() ) return 1;
    
    temp_y = Generic.y + Generic.height;
    temp_y -= 2;
    if(bg_collision_sub() ) return 1;
    
    return 0;
}

char bg_coll_R(void){
    // check 2 points on the right side
    temp_x = Generic.x + Generic.width;
    
    eject_R = (temp_x + 1) & 0x0f;
    temp_y = Generic.y + 2;
    if(bg_collision_sub() ) return 1;
    
    temp_y = Generic.y + Generic.height;
    temp_y -= 2;
    if(bg_collision_sub() ) return 1;
    
    return 0;
}

char bg_coll_U(void){
    // check 2 points on the top side
    temp_x = Generic.x + 2;
    
    temp_y = Generic.y;
    eject_U = temp_y | 0xf0;
    if(bg_collision_sub() ) return 1;
    
    temp_x = Generic.x + Generic.width;
    temp_x -= 2;

    if(bg_collision_sub() ) return 1;
    
    return 0;
}

char bg_coll_D(void){
    // check 2 points on the bottom side
    temp_x = Generic.x + 2;

    temp_y = Generic.y + Generic.height;
    eject_D = (temp_y + 1) & 0x0f;
    if(bg_collision_sub() ) return 1;
    
    temp_x = Generic.x + Generic.width;
    temp_x -= 2;

    if(bg_collision_sub() ) return 1;
    
    return 0;
}



char bg_collision_sub(void){
	if(temp_y >= 0xf0) return 0;
	
	coordinates = (temp_x >> 4) + (temp_y & 0xf0);
	
	collision = c_map[coordinates];
	
	return collision;
}


