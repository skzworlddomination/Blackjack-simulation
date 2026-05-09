all: blackjack
blackjack: blackjack_game.c
	gcc -o blackjack blackjack_game.c
