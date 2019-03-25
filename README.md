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

## v0.8 target
## 洗牌:mallard
#### 修改table_state
```enum class status_fields : uint64_t
        {
            ROUND_START = 1,
            ROUND_BET = 2,
            ROUND_REVEAL = 4,
            ROUND_END = 0,
            ROUND_SHUFFLE = 6,
            PAUSED = 3, // must be changed under ROUND_END status.
            CLOSED = 5
        };
```
#### verserverseed修改
- (validCardVec.size() <= CardsMinLimit)
    - tableStatus = ROUND_SHUFFLE;

#### add table shuffle_info
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

#### table_stats表中增加cardboot字段
#### 将开奖流程单独提取出来

#### add sc::shuffle
- 初始化416张牌
- 随机生成416以内的随机数，并获取validCardVec中对应的牌的点数
- 随机抽取上一步中对应点数的牌数 并删除
- 删除shufflez_info中对应tableId的数据
- 开奖三次，并把对应的结果保存到shuffle_info表中
- table_stats对应的cardboot字段加一，并将tableStatus = ROUND_END

## 桌属性
### 增加默认常量
    - 最小抵押额 minPerBet_default
    - comission_rate_platform_default = 2/1000

### newtable
- 修改原来所有 min max参数，都没有默认值（代码中判断 只要有一个为0 就中asset中断）
- 增加参数 commission_rate_agent 和 commission_rate_player （这两个参数没有默认值）
- oneRoundDealerMaxPay_temp = oneRoundMaxTotalBet_Push_temp * 11 * 2 + max(oneRoundMaxTotalBet_BP_temp * 1, oneRoundMaxTotalBet_Tie_temp * 8) + delerbalance*(comission_rate_platform_default + commission_rate_agent + commission_rate_player);

### add sc::edittable(uint64_t tableId)

## 反佣逻辑
- player/agent/platform 反佣全部由dealerbalance出资

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

