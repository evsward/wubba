# wubba
## v0.1-v0.3 BM_rand + round
- v0.1 base BM_rand. 0-1, tag release.
    - ~~v0.1.2 workflow control. Timely and step dependency.~~
    - ~~v0.1.3 token~~ 
    - ~~v0.1.4 multi_P _~~
    - v0.1.5 detailed validation
- v0.2 base game SC. tag release.
    - 0.2.1 change to detail game logic: six seeds.
    - 0.2.2 card boot
    - 0.2.3 result parser.
    - ~~0.2.4 new table~~
    - 0.2.5 table/game list
    - ~~0.2.6 trusteeship/disconnect~~
    - 0.2.7 agent workflow _
- v0.3 base game version complete. tag release.
## v0.4 baccarat target
### (1) player bet
- Action params construction. 
> eg. ("100.0000 SYS", "0.0000 SYS", "3.0000 SYS", "4.0000 SYS", "3.0000 SYS")

- Param parser, rules order: 
> betDealer, betPlayer, betTie, betDealerPush, betPlayerPush

- Multi_index table construction. (betType, betAmount) => 
```
player_info
{
    betDealer = "100.0000 SYS"
    betPlayer = "0.0000 SYS"
    betTie = "3.0000 SYS"
    betDealerPush = "4.0000 SYS"
    betPlayerPush = "3.0000 SYS"
}
```
### (2) card boot
- ~~bootArray, fix size: 416, index range [0, 415]. Once card used, the value changed to -1.~~
- validCardArr, storage the index of valid cards in bootArray as value. Once card used, delete the element and the size of array minus one.++**vector.erase()**++
- Six seeds. Generated by the root seed parameter. Get a 64 chars though SHA256. Every 9 chars make up one seed in order. (10 chars unused at last.)++**SUBSTRING**++
- Card parser in bootArray.
    - card number
    - Poker suit 
```math
cardnumber = (index+1) % 13
```
```math
suitcolor = (index+1)/13 % 4
```
**result parser**:

flag | symbol | full name
---|---|---
0 | S | Spades
1 | H | Hearts
2 | D | Diamonds
3 | C | Clubs

```

```

### (3) reveal
- card apply, 4 cards, or 5 cards, or 6 cards? 
> Refer to the rules table:

![image](https://github.com/evsward/wubba/blob/master/resouce/cardapplyrules.jpg?raw=true)

- who win? dealer, player, tie, dealerPush or playerPush? or multy win. 
```math
points = sum of all cards %10
```
- odds token. 

type | odds
---|---
dealer | 1:0.95
player | 1:1
tie | 1:8
dealerPush, playerPush | 1:11

#### state tableround field add:
- result: round result, who win?
- player\_bet\_info add:
    - playerBonus, token amount transfer to player.
    - dealerBonus. add into dealderBalance.

### (4) notices
- ~~Bet recall function is implemented on the server end, not SC. Server-end keep the bet cache.~~
- All actions of SC need both operator and server account authority verification. ++**TEST**++
- Tie determined by the first two cards. 
- Add conditions, delete **minPerBet, oneRoundMaxTotalBet**:
```
    oneRoundMaxTotalBet_BP;   // (1000)max of banker and player total bet. 
    minPerBet_BP;             // (100)min of per banker or player bet.
    oneRoundMaxTotalBet_Tie;  // (100)max of tie total bet.
    minPerBet_Tie;            // (10)min of per tie bet.
    oneRoundMaxTotalBet_Push; // (50)max of push total bet.
    minPerBet_Push;           // (1)min of per push bet.
```

```math
oneRoundDealerMaxPay = oneRoundMaxTotalBet_Push*11*2 + max(oneRoundMaxTotalBet_BP*1, oneRoundMaxTotalBet_Tie*8)
```
```math
minTableDeposit = oneRoundDealerMaxPay*minTableRounds
```
- Add verification: verify if dealerBalance > oneRoundDealerMaxPay*2 is false, can't start a new round and call inline action::pausetable.
- Add actions: 
    1. pausetable (can recover by continuetable)
    1. continuetable
    1. closetable (can't recover)

## v0.5 target
### random result uniform distribution
- Need a big data analysis model by python script. 
- Adjust variate of the solution to obtain a best result.
- Seed source.

## v0.6 target
### commission flow
- dealer
- platform
- agent

## v0.6.1 target
agent workflow.
