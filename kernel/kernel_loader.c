// kernel.c
void kernel_main() {
    // Clear screen
    char* video = (char*)0xB8000;
    for(int i = 0; i < 80*25*2; i++) {
        video[i] = 0;
    }

    // Print message
    video[0] = 'H';
    video[1] = 0x1F;  // Xanh trên đỏ
    video[2] = 'I';
    video[3] = 0x1F;

    while(1);
}