#include<stdio.h>

const int STANDARD = 150;
const int PREMIUM = 250;
const int RECLINER = 400;

const char* TicketCategory(int category) {
    switch (category) {
        case 1:
            return "Standard";
        case 2:
            return "Premium";
        case 3:
            return "Recliner";
    }
}
float TotalAmount(int totalTickets , int ticketCategory ){
    float amount = 1.0f;
    switch (ticketCategory) {
        case 1:
            return amount * totalTickets * STANDARD;
        case 2:
            return amount * totalTickets * PREMIUM;
        case 3:
            return amount * totalTickets * RECLINER;
        
    }

}
float applyDiscount(float totalAmount){
    float discount = totalAmount / 10;
    return totalAmount - discount;
}
int main(){
    char name[50];
    int totalTickets;
    int ticketCategory;

    float totalAmount;
    float discount;
    float finalAmount;
    

    scanf("%s",name);
    scanf("%d",&totalTickets);
    scanf("%d",&ticketCategory);
    const char *ticket = TicketCategory(ticketCategory);
    
    totalAmount = TotalAmount(totalTickets ,  ticketCategory);

    if(totalTickets >= 5){
        finalAmount = applyDiscount(totalAmount);
        discount = totalAmount / 10.0f;
    }else{
        finalAmount = totalAmount;
        discount = 0.0f;
    }
    printf("Customer : %s\n",name);
    printf("Ticket Category : %s\n",ticket);
    printf("Tickets : %d\n",totalTickets);
    printf("Subtotal : %.2f\n",totalAmount);
    printf("Discount : %.2f\n",discount);
    printf("Final Bill: %.2f\n",finalAmount);

    return 0;
}