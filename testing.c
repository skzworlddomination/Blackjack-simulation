#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    int rank;
    char suit;
} Card;

typedef struct {
    Card cards[104];
    int top;
    int size;
} Shoe;

typedef struct {
    Card cards[12];
    int count;
} Hand;

void init_shoe(Shoe *shoe);
void shuffle_shoe(Shoe *shoe);
Card draw_card(Shoe *shoe);

void add_card(Hand *hand, Card c);
int hand_value(Hand *hand);
void print_hand(Hand *hand);
void print_card(Card c);
int card_value(int rank);

double bust_probability(Hand *hand, Shoe *shoe);
double win_probability(Hand *player, Card dealer_up, Shoe *shoe);
double dealer_finish_probability(int player_total, int dealer_total, int dealer_aces, int counts[], int cards_left);

int main(void) {
    system("chcp 65001 > nul");
    srand(time(NULL));

    int repeat = 1;

    while (repeat == 1) {

        Shoe shoe;
        Hand player = { .count = 0 };
        Hand dealer = { .count = 0 };

        int player_busted = 0;

        init_shoe(&shoe);
        shuffle_shoe(&shoe);

        add_card(&player, draw_card(&shoe));
        add_card(&dealer, draw_card(&shoe));
        add_card(&player, draw_card(&shoe));
        add_card(&dealer, draw_card(&shoe));

        while (1) {
            printf("\nDealer shows:\n");
            print_card(dealer.cards[0]);
            printf("\n");

            printf("Your hand: \n");
            print_hand(&player);

            int total = hand_value(&player);
            printf("Total: %d\n", total);

            double bust = bust_probability(&player, &shoe);
            double win = win_probability(&player, dealer.cards[0], &shoe);

            printf("Chance of bust if you hit: %.2f%%\n", bust * 100);
            printf("Chance of winning if you stay: %.2f%%\n", win * 100);

            if (total > 21) {
                printf("BUST!\n");
                player_busted = 1;
                break;
            }

            

            printf("Hit (1) or Stay (0)? ");
            int choice;
            scanf("%d", &choice);

            if (choice == 0){
                break;
            }

            add_card(&player, draw_card(&shoe));
        }

        if (!player_busted && hand_value(&player) <= 21) {

            printf("\nDealer hand: \n");
            print_hand(&dealer);

            while (hand_value(&dealer) < 17) {
                Card c = draw_card(&shoe);
                add_card(&dealer, c);
                printf("Dealer draws: \n");
                print_card(c);
                printf(" (total: %d)\n", hand_value(&dealer));
            }

            int p = hand_value(&player);
            int d = hand_value(&dealer);

            printf("\nFinal totals - You: %d Dealer: %d\n", p, d);

            if (d > 21 || p > d) printf("You win!\n");
            else if (p < d) printf("Dealer wins!\n");
            else printf("Push!\n");
        }

        printf("\nPlay again?\nYes (1)\nNo (0)\n");
        scanf("%d", &repeat);
    }

    return 0;
}


void init_shoe(Shoe *shoe) {
    char suits[] = {'H', 'D', 'C', 'S'};
    int index = 0;

    for (int deck = 0; deck < 2; deck++) {
        for (int s = 0; s < 4; s++) {
            for (int r = 1; r <= 13; r++) {
                shoe->cards[index].rank = r;
                shoe->cards[index].suit = suits[s];
                index++;
            }
        }
    }

    shoe->top = 0;
    shoe->size = index;
}

void shuffle_shoe(Shoe *shoe) {
    for (int i = shoe->size - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        Card temp = shoe->cards[i];
        shoe->cards[i] = shoe->cards[j];
        shoe->cards[j] = temp;
    }
}

Card draw_card(Shoe *shoe) {
    return shoe->cards[shoe->top++];
}

void add_card(Hand *hand, Card c) {
    hand->cards[hand->count++] = c;
}

int hand_value(Hand *hand) {
    int total = 0;
    int aces = 0;

    for (int i = 0; i < hand->count; i++) {
        int r = hand->cards[i].rank;

        if (r == 1) {
            total += 11;
            aces++;
        } else if (r >= 11) {
            total += 10;
        } else {
            total += r;
        }
    }

    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }

    return total;
}

const char* rank_str(int r) {
    static char buf[3];

    if (r == 1) return "A";
    if (r == 11) return "J";
    if (r == 12) return "Q";
    if (r == 13) return "K";

    sprintf(buf, "%d", r);
    return buf;
}

const char* suit_str(char s) {
    if (s == 'H') return "♥";
    if (s == 'D') return "♦";
    if (s == 'C') return "♣";
    if (s == 'S') return "♠";
    return "?";
}

void print_hand(Hand *hand) {
    int n = hand->count;

    // top border
    for (int i = 0; i < n; i++) printf("+--------+ ");
    printf("\n");

    // top rank
    for (int i = 0; i < n; i++)
        printf("|%-2s      | ", rank_str(hand->cards[i].rank));
    printf("\n");

    for (int i = 0; i < n; i++)
        printf("|        | ");
    printf("\n");

    // suit
    for (int i = 0; i < n; i++)
        printf("|   %s    | ", suit_str(hand->cards[i].suit));
    printf("\n");

    for (int i = 0; i < n; i++)
        printf("|        | ");
    printf("\n");

    // bottom rank
    for (int i = 0; i < n; i++)
        printf("|      %-2s| ", rank_str(hand->cards[i].rank));
    printf("\n");

    // bottom border
    for (int i = 0; i < n; i++) printf("+--------+ ");
    printf("\n");
}





void print_card(Card c) {
    const char *r = rank_str(c.rank);
    const char *s = suit_str(c.suit);

    printf("+--------+\n");
    printf("|%-2s      |\n", r);
    printf("|        |\n");
    printf("|   %s    |\n", s);
    printf("|        |\n");
    printf("|      %-2s|\n", r);
    printf("+--------+\n");
}


int card_value(int rank) {
    if (rank == 1) {
        return 11;
    }
    if (rank >= 11) {
        return 10;
    }
    return rank;
}

double bust_probability(Hand *hand, Shoe *shoe) {
    int busts = 0;
    int remaining = shoe->size - shoe->top;

    for (int i = shoe->top; i < shoe->size; i++) {
        Hand temp = *hand;
        add_card(&temp, shoe->cards[i]);
        if (hand_value(&temp) > 21) {
            busts++;
        }
    }

    if (remaining == 0) {
        return 0.0;
    }

    return (double)busts / remaining;
}

double win_probability(Hand *player, Card dealer_up, Shoe *shoe) {
    int player_total = hand_value(player);
    int counts[14] = {0};
    int cards_left = shoe->size - shoe->top;
    double win = 0.0;

    if (player_total > 21 || cards_left == 0) {
        return 0.0;
    }

    for (int i = shoe->top; i < shoe->size; i++) {
        counts[shoe->cards[i].rank]++;
    }

    for (int hidden = 1; hidden <= 13; hidden++) {
        if (counts[hidden] == 0) {
            continue;
        }

        counts[hidden]--;

        int dealer_total = card_value(dealer_up.rank) + card_value(hidden);
        int dealer_aces = (dealer_up.rank == 1) + (hidden == 1);

        while (dealer_total > 21 && dealer_aces > 0) {
            dealer_total -= 10;
            dealer_aces--;
        }

        win += ((double)(counts[hidden] + 1) / cards_left) * dealer_finish_probability(player_total, dealer_total, dealer_aces, counts, cards_left - 1);

        counts[hidden]++;
    }

    return win;
}

double dealer_finish_probability(int player_total, int dealer_total, int dealer_aces, int counts[], int cards_left) {
    if (dealer_total > 21) {
        return 1.0;
    }

    if (dealer_total >= 17 || cards_left == 0) {
        return player_total > dealer_total ? 1.0 : 0.0;
    }

    double win = 0.0;

    for (int rank = 1; rank <= 13; rank++) {
        if (counts[rank] == 0) {
            continue;
        }
        counts[rank]--;
        int next_total = dealer_total + card_value(rank);
        int next_aces = dealer_aces + (rank == 1);

        while (next_total > 21 && next_aces > 0) {
            next_total -= 10;
            next_aces--;
        }
        win += ((double)(counts[rank] + 1) / cards_left) * dealer_finish_probability(player_total, next_total, next_aces, counts, cards_left - 1);
        counts[rank]++;
    }
    return win;
}
