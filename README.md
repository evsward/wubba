# Devlog [CURRENT](https://github.com/evsward/wubba#v083-target)
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
## v0.4 mallard target
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
    - shuffle function
- Six seeds. Generated by the root seed parameter. Get a 64 chars via SHA256. Every 9 chars make up one seed in order. (10 chars unused at last.)++**SUBSTRING**++
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

### (3) reveal
- card apply, 4 cards, or 5 cards, or 6 cards? 
    - player_hands
    - banker_hands
> Refer to the rules table:

![image](https://github.com/evsward/wubba/blob/master/resource/cardapplyrules.jpg?raw=true)

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
    - playerBonus, token amount transfer to player. minus from dealerBalance.
    - dealerBonus. add into dealderBalance.

### (4) notices
- ~~Bet recall function is implemented on the server end, not SC. Server-end keep the bet cache.~~
- All actions of SC need both operator and server account authority verification. ++**TEST**++
- Push determined by the first two cards. 
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

**NOTICE: Checking all transfer actions, if the amount == 0, don't transfer.**

- Smart contract account's operation: delete all of table closed.
- Add a global variable: **CardsMinLimit**, initial with 100.
- Add a bool field in table-stats: **isPrivate**. If it's true, UI could hidden this table in list.
- Add an ACTION depositable: allow dealer to supply deposit for its existing table. 
    - **NOTE**: AUTO START: if table status == PAUSED, change it to ROUND_END.
    - params: (uint64_t tableId, name dealer, asset deposit)
- Add an ACTION changeprivat: allow dealer to change the **isPrivate** field via this action.
- Change ACTION newtable params:

parameter | required | default
---|---|---
name dealer | *required | 
asset deposit | *required | 
bool isPrivate | *option | 0 (public)
asset oneRoundMaxTotalBet_BP | *option | 1000
asset minPerBet_BP | *option | 100
asset oneRoundMaxTotalBet_Tie | *option | 100
asset minPerBet_Tie | *option | 1
asset oneRoundMaxTotalBet_Push | *option | 50
asset minPerBet_Push | *option | 1

- Add an ACTION dealerWitdaw: allow dealer to withdraw from dealerBalance. 
    - **NOTE**: Must keep dealerBalance >= oneRoundDealerMaxPay*2 at least.
    - **NOTE**: If dealer want to withdraw all, close the table.
- Change ACTION closetable: transfer all the dealerBalance to dealer account before closing this table. 
    - **NOTE**: table closed can't be recovery any more.

## v0.6 target
Add lizard.
- modify 3 ACTION: newtable, playerbet, verserveseed(reveal)
- modify the state table structure. 

### newtable
Modify the params: (name dealer, asset deposit, bool isPrivate)
### playerbet
Modify the params: (uint64_t tableId, name player, string bet)
> string bet

This is a JSON.  **JSON resolver in SC**

**Betting options**  

name | code | odds | notes
---|---|---|---
big | big | 1:1 | score =[11,17] without triple
small | small |  1:1 | score =[4,10] without triple
odd | odd |  1:1 | score =odd without triple
even | even |  1:1 | score =even without triple
any triple | anytri |  1:24 | 
specific triple | tri1/tri2/tri3/tri4/tri5/tri6 |  1:150 | 
specific double | pair1/pair2/pair3/pair4/pair5/pair6 |  1:8 | 
total(4,17) | total4/total17 | 1:50 | 
total(5,16) | total5/total16 | 1:18 | 
total(6,15) | total6/total15 | 1:14 | 
total(7,14) | total7/total14 | 1:12 | 
total(8,13) | total8/total13 | 1:8 | 
total(9,12) | total9/total12 | 1:6 | 
total(10,11) | total10/total11 | 1:6 | 
two combinations | c12/c13/c14/c15/c16/c23/c24/c25/c26/c34/c35/c36/c45/c46/c56 | 1:5 | 
single | s1/s2/s3/s4/s5/s6 | 1:1 | 

JSON data structure example：
```
{
    "small": "0.5000 SYS",
    "total6": "2.5000 SYS",
    "tri2": "10.0000 SYS"
}
```
### verserveseed(reveal)
- 1 diceResult  

root_seed split to 3 sub_seeds. 
> diceNumber = random(sub_seed) % 6 + 1

> score = sum of 3 diceNumbers.

diceResult data structure example：
```
diceResult="345"
```

- 2 roundResult  

parse to **Betting options**  
roundResult data structure example：
```
diceResult="222"
roundResult=["small","even","anytri","tri2","pair2","total6","s2"]
```
size(roundResult) = 7/9

- 3 odds redeem  
    - **pBonus** (odds chose, lots of judgments and loops.)
    - dBouns

Traverse player_bet_info    
According to the odds in the **Betting options** table as above, calculate pBonus/dBonus.  Transfer to player with pBonus and add to **dealerBalance** with dBouns.

### state table structure
- table fields: uint64_t tableId, name dealer, bool trusteeship, bool isPrivate, asset dealerBalance
- round fields: 

    uint64_t betStartTime  
    uint64_t tableStatus  
    checksum256 dealerSeedHash  
    checksum256 serverSeedHash  
    string dealerSeed  
    string serverSeed  
    bool dSeedVerity  
    bool sSeedVerity  
    std::vector<player_bet_info> playerInfo  
    string diceResult  
    string roundResult

- player_bet_info

    name player  
    string bet  (JSON)  
    asset pBonus  
    asset dBonus  

## v0.6.1 target
- **newtable** set the betPerMin/oneRoundMaxTotalBet. (string betConfig)
- Abstract symbol as a variable could be set. ("SYS","EOS" etc.)
- Delete unused code/include.
### remove unused codes
remove unused codes inherit from mallard.

### support any currency（default SYS）

- Add betPerMin/oneRoundMaxTotalBet control. (including **currRoundBetSum** in table state.)

**betPerMin/oneRoundMaxTotalBet control**  

name | odds | include| betPerMin_(name) | oneRoundMaxTotalBet_(name)
---|---|---|---|---
bsoe | 1:1 | big,small,odd,even | 10.0000 SYS | 3000.0000 SYS
anytri | 1:24 |  anytri | 0.5000 SYS | 400.0000 SYS
trinum | 1:150 |  tri1/tri2/tri3/tri4/tri5/tri6 | 0.1000 SYS | 100.0000 SYS
pairnum | 1:8 |  pair1/pair2/pair3/pair4/pair5/pair6 | 1.0000 SYS | 1000.0000 SYS
txx | 1:50/18/14/12/8/6 |  totalxx | 0.5000 SYS | 500.0000 SYS
twocom | 1:5 |  c12/c13/c14/c15/c16/c23/c24/c25/c26/c34/c35/c36/c45/c46/c56 | 1.0000 SYS | 1500.0000 SYS
single | 1:1 |  s1/s2/s3/s4/s5/s6 | 10.0000 SYS | 3000.0000 SYS

```math
diff_max = oneRoundMaxTotalBet_bsoe*2 + oneRoundMaxTotalBet_txx*14 + oneRoundMaxTotalBet_twocom*5*3 + oneRoundMaxTotalBet_single*3
```

```math
pair_nontri_max = oneRoundMaxTotalBet_bsoe*2 + oneRoundMaxTotalBet_pairnum*8 + oneRoundMaxTotalBet_txx*50 + oneRoundMaxTotalBet_twocom*5 + oneRoundMaxTotalBet_single*2
```

```math
tri_max = oneRoundMaxTotalBet_anytri*24 + oneRoundMaxTotalBet_trinum*150 + oneRoundMaxTotalBet_pairnum*8 + oneRoundMaxTotalBet_txx*14 + oneRoundMaxTotalBet_single*1
```

```math
oneRoundDealerMaxPay = max(diff_max, pair_nontri_max, tri_max)
```

betPerMin/oneRoundMaxTotalBet naming  rule, for example:

> betPerMin_bsoe, betPerMin_anytri ... use betPerMin add the name of "control table" as above.

- currRoundBetSum_bsoe, currRoundBetSum_anytri ...

- Modify the control of:
    - asset oneRoundDealerMaxPay;
    - asset minTableDeposit;

## v0.7 target
agent workflow.
### add a new state table
```
struct alias_info
{
    string alias;          
    name account;  
}
```
### SC::pushaliasnam(string alias, name account)
Insert the alias info into state table "alias-info".

### SC::playerbet modify
Add param: 
- name agent, client get EOS account by given alias through search the "alias-info".
- string nickname, client allow user to input a nickname just for showing instead of 12 EOS account name.

### state table: tableround_info modify
Add fields in struct player_info_bet:
- name agent, will be used in reveal stage.
- string nickname, for showing.(do not care about repetition)

### platform commssion
```math
platformtotransfer = (pBonus + dBonus)*2/1000
```
```
pBonus = pBonus*998/1000;
dBonus = dBonus*998/1000;
```
> default commission rate of platform: 2/1000

### agent commission
Reveal stage, got player-bet obj:
- pBonus
- dBonus
- agent, if agent exist in "alias-info" && agent != dealer, return the commission to agent account.

transfer type(if dBonus>0, agent got commssion):
```math
agenttotransfer = dBonus*5/100
```
> default commission rate of agent: 5/100

## v0.7.1 target
### 1.shuffle(mallard)
#### table status modify
```enum class status_fields : uint64_t
        {
            ROUND_START = 1,
            ROUND_BET = 2,
            ROUND_REVEAL = 4,
            ROUND_END = 0,
            ROUND_SHUFFLE = 6, // shuffle stage for mallard game.
            PAUSED = 3, // must be changed under ROUND_END status.
            CLOSED = 5
        };
```
#### sc::verserverseed modify
reveal stage, end of this sc:
- if (validCardVec.size() <= CardsMinLimit)
    - tableStatus = ROUND_SHUFFLE;

#### add state table: shuffle_info
```
TABLE shuffle_info
    {
        uint64_t tableId;
        uint64_t roundNum;
        string roundResult;
        std::vector<card_info> playerHands;
        std::vector<card_info> bankerHands;
    };
```

#### table_stats add a field: cardboot(init/default with 1)
#### abstract the reveal logic as a function, used by normal workflow and shuffle 3 non-bet workflow.
#### add sc::shuffle
- Re construct 416 size vector.
- Random one card, according to the point of this card, random the same amount cards and delete them.
- Empty the item of current tableId in table:shuffle_info.
- Call reveal function 3 times without bet and settlement. Storage the result into table:shuffle_info.
- table_stats->cardboot +1;
- tableStatus = ROUND_END;

### 2.table fields(two games)
#### add global const var:
- comission_rate_platform_default = 2/1000

#### modify sym_info
- add one field: **minPerBet_default**

#### newtable
- Delete all the default value of params with "min" and "max", which set by dealer. Assert the params' value >0 and assert the mininum value must great than or equal to **minPerBet_default**.
- Add two agent related params, which have no default value and set by dealer, >=0, storage them into 'tablesinfo':
    - commission_rate_agent
    - commission_rate_player
- mallard::oneRoundDealerMaxPay_temp += (oneRoundMaxTotalBet_tie + oneRoundMaxTotalBet_bp + oneRoundMaxTotalBet_push)*(comission_rate_platform_default + commission_rate_agent + commission_rate_player);
- lizard::oneRoundDealerMaxPay_temp += (oneRoundMaxTotalBet_bsoe + oneRoundMaxTotalBet_anytri + oneRoundMaxTotalBet_trinum + oneRoundMaxTotalBet_pairnum + oneRoundMaxTotalBet_txx + oneRoundMaxTotalBet_twocom + oneRoundMaxTotalBet_single)*(comission_rate_platform_default + commission_rate_agent + commission_rate_player);

#### add sc::edittable
- mallard: uint64_t tableId, bool isPrivate, name code, string sym, asset oneRoundMaxTotalBet_bp, asset minPerBet_bp,asset oneRoundMaxTotalBet_tie, asset minPerBet_tie,asset oneRoundMaxTotalBet_push, asset minPerBet_push
- lizard: uint64_t tableId, bool isPrivate, name code, string sym, asset oneRoundMaxTotalBet_bsoe, asset minPerBet_bsoe, asset oneRoundMaxTotalBet_anytri, asset minPerBet_anytri, asset oneRoundMaxTotalBet_trinum, asset minPerBet_trinum, asset oneRoundMaxTotalBet_pairnum, asset minPerBet_pairnum, asset oneRoundMaxTotalBet_txx, asset minPerBet_txx, asset oneRoundMaxTotalBet_twocom, asset minPerBet_twocom, asset oneRoundMaxTotalBet_single, asset minPerBet_single

### 3.commission logic
SC::playerbet add a commission settlement logic:
- player/agent/platform, all paid by the banker(**dealerBalance**).

## v0.8 target
Requirement change, workflow：
> ①new idea -> ②analyze -> ③add plan here(markdown) -> **④coding** -> **⑤testing** -> **⑥commit to github** -> ⑦verify **⑧modify interface file** -> **⑨commit** -> ⑩verify -> **⑪send to client colleague** -> **⑫explain**

- ①②③⑦⑩：web
- ④⑤⑥⑧⑨⑪⑫: liS

### 1. sym_info
Put the sym_info records into On-chain state table.
- create a new state table obj: currencyinfo
- adjust sym_info struct: {code[pk], symName, minPerBet_default}
- add SC:init
    - empty parameter
    - permission: contract account.
    - insert the init support currency token type.
- test if successfull when find item from by code.
- test get table.

### 2. fix max tables per dealer
- Add a limit of the maximum number of tables that can be created per person.
- Set a global var named "maxinum_table_per_dealer", default as 100.
- SC::newtable need to verify if the tables owned by current dealer is over the "maxinum_table_per_dealer".
- Exclude closed tables.

### 3. aliasinfo account unique
- Null or "" values are not accepted in SC::pushaliasnam. 
- Account in aliasinfo must be unique.

### 4. fix bug: deposit assert
- dealerBalance + deposit >= minTableDeposit

### 5. adminaccount
- add a new global var named: adminaccount = "useraaaaaaak"

### 6. modify the important interface's permission
- ***erasingdata***, change to **adminaccount**
- ***initsymbol***, change to **adminaccount**
- ***shuffle***, bac specific. Change to **serveraccount**

### 7. modify erasingdata content
- modify the name from ***erasingdata*** to ***clear12cache***.
- delete content: delete one item according to the key{tableId}.
- modify the key which could delete all(used when update SC with data changed). From -1 to one gloabl var named: delall_key = 103718369455

### 8. modify initsymbol
- add verification: transfer *minperbet* new symbol added to verify from ***adminaccount*** to ***contractaccount***. If success, insert symbol to **currencyinfo**, otherwise break off.

### 9. modify tablesinfo strut
- add two new fields: *playercommission*, *agentcommission* into the **player_bet_info**
- modify the SC:playerbet, record the *playercommission* and *agentcommission* into **tablesinfo** per bet.

## v0.8.1 target
- ~~combine dealerseed serverseed~~  ***RECOVER***
- add salt in reveal root_seed before sha256.
- permission adjust: all dealer action add server permission additional.
- deposittable modify: delete the first param: name dealer.
- clear12cache: can only delete tables whose status are ROUND_END or PAUSE 

## v0.8.2 import12data
- add new interface:***upgrading***, permission:**adminaccount** with one param {bool flag}. 
    - If true, {!existing->trusteeship, dealerseed} and {existing->trusteeship, serverseed} can't start a new round. 
- add new interface: ***import12data***, permission:**adminaccount**. Importing the current SC's state data struct with one item.

## v0.8.3 target
- fix bug: initsymbol verify token that not exsit should be failed.
- remove all alias code including: 
    - bet param{agent} change to name type
    - aliasinfo state table
    - delete SC::pushaliasnam
    - **NOTE:**code clean and testing.

## TODO
### exercise: update sc with data struct change
1. Add one redundancy fields
2. Do step in *sc_deploy_update.docx*

**repeat 1,2 with 3 times at least.** 

---

# NOTE
## strategy:udpate SC with state data struct modification 
- edit SC state data struct and modify ***import12data*** with new data strut.
- localDB via getTableRows storage all exsit data items.
- server account call ***erasingdata***
- server account call ***import12data*** with all items storaged in localDB.

## spreadcode
### alias_info lifetime
> rules: delete the item obj in "alias_info" when the item inactive for one month.

TODO: add one field "lastoptime" in "alias_info", add one option in SC::erasingdata, batch delete 
> now - lastoptime > 30 days.

### spreadcode generate
- create an alias of EOS account A.
- if A is a dealer, spreadcode = 
    - "alias[A]"
    - "alias[A]-gameid-tableid"
- if A want to advertise the other dealer B, spreadcode = 
    - "alias[A]-alias[B]"
    - "alias[A]-alias[B]-gameid-tableid"
> if alias[B] is not exsit, use B.

## seed server
Nodejs-based server-side executable program load balanced with localDB.

### data structure

field | explaination
---|---
tableId | unique identifier, equal with table-state's tableId.
seed | Locally acquired random number seeds.
seedHash | result of hash seed
    
### Execute {get table} per second to get all tables' state data. Filter with:
- ROUND_END&&trusteeship  || ROUND_START&&!trusteeship 
    - table-obj -> tableId, acquired local seed, hash seed. Insert {tableId, seed, seedHash} into localDB.
    - call SC: serverseed(tableId，seedHash)
- ROUND_REVEAL
    - table-obj -> tableId, data obj from localDB with tableId, acquired plaintext seed.
    - call SC: verserveseed(tableId，seed)
    - delete the data obj from localDB with the tableId handled.

> If one of the seed servers was attacked, load balancer could switch another server automated . If the time point of attack was during bet stage(ROUND_BET), SC could't acquire serverseed when reveal stage. The result of this round is based on the dealerseed.

### TODO
- Use dealerseed to reveal when serverseed not exsit.
- Return all bet amount when no one seed in reveal stage. (both dealer seed and server seed are not exsit)

## random result uniform distribution
- Need a big data analysis model by python script. 
- Adjust variate of the solution to obtain a best result.
- Seed source.

