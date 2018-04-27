// Basic raytracer kernel version
void kernel color(global unsigned char* colorBuffer, int nx, int ny) { 
    // get the pixel coords
    int x = get_global_id(0);
    int y = get_global_id(1);

    // write the color red to the buffer
    int index = x + y * nx;
    colorBuffer[3 * index + 0] = 255;
    colorBuffer[3 * index + 1] = 100;
    colorBuffer[3 * index + 2] = 100;
}
