// kernel/memory.c
#include "memory.h"

void memory_init(void){
    // TODO: Khởi tạo hệ thống quản lý bộ nhớ (heap/paging)
}
void* kmalloc(size_t size){
    (void)size; // Ghi đè kiểu (cast) tham số size thành void để báo cho compiler biết nó là "có chủ ý không sử dụng"
    // TODO: Triển khai logic cấp phát
    return NULL;
}

void kfree(void* ptr){
    (void)ptr;  // Ghi đè kiểu tham số ptr thành void
    // TODO: Triển khai logic giải phóng
}