#include "cache.h"

uint32_t CACHE::find_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    // baseline LRU replacement policy for other caches 
    return lru_victim(cpu, instr_id, set, current_set, ip, full_addr, type); 
}

void CACHE::update_replacement_state(uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    if (type == WRITEBACK) {
        if (hit) // wrietback hit does not update LRU state
            return;
    }

    return lru_update(set, way);
}

uint32_t CACHE::lru_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    uint32_t way = 0;

    // fill invalid line first
    for (way=0; way<NUM_WAY; way++) {
        if (block[set][way].valid == false) {

            DP ( if (warmup_complete[cpu]) {
            cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " invalid set: " << set << " way: " << way;
            cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
            cout << dec << " lru: " << block[set][way].lru << endl; });

            break;
        }
    }
    
    // LRU victim
    uint32_t j=NUM_WAY-1,a,x=1;//,wayf=1;
    if (way == NUM_WAY) {
        if(NAME=="LLC"){
            while(x==1){
                for (way=0; way<NUM_WAY; way++) {
                    if (block[set][way].lru == j) {
                        
                        if(cpu!=block[set][way].cpu){
                            cout << "Different Cores!" << endl;
                            cout << "Evicting CPU : " << cpu << "   Block CPU : " << block[set][way].cpu << endl;
                            a=evict_from_parent(block[set][way].address,instr_id,cpu);
                            if(a==1){
                                x=1;
                                j--;
                                cout << "Checking the next block with LRU " << block[set][way].lru << endl << endl;
                                break;
                            }
                            if(a==0){
                                x=0;
                                j--;
                                if(j!=15){
                                    cout << "It's own block " << block[set][way].lru << " is being evicted " << endl << endl;
                                    break;
                                }
                            }
                        }
                        else{
                            cout << "Same Core!" << endl;
                            cout << "It's own block " << block[set][way].lru << " is being evicted " << endl << endl;
                            x=0;
                            if(j!=15){
                                x=0;
                            }
                            break;
                        }
                        
                        DP ( if (warmup_complete[cpu]) {
                        cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                        cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                        cout << dec << " lru: " << block[set][way].lru << endl; });
        
                        //break;
                    }

                }
                if(block[set][way].lru==12){
                    cout << "Randomly Evicting  " << block[set][way].lru << endl << endl;
                    x=1;
                    j=15;
                    break;
                }
            }
        }
        else{
            for (way=0; way<NUM_WAY; way++) {
                if (block[set][way].lru == NUM_WAY-1) {
                    
                    //cout << NAME << "  LRU " << block[set][way].lru << " found at WAY " << way << endl << endl;;
                    
                    DP ( if (warmup_complete[cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                    cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                    cout << dec << " lru: " << block[set][way].lru << endl; });
    
                    break;
                }
            }
        }
    }

    if (way == NUM_WAY) {
        cerr << "[" << NAME << "] " << __func__ << " no victim! set: " << set << endl;
        assert(0);
    }

    return way;
}

void CACHE::lru_update(uint32_t set, uint32_t way)
{
    // update lru replacement state
    for (uint32_t i=0; i<NUM_WAY; i++) {
        if (block[set][i].lru < block[set][way].lru) {
            block[set][i].lru++;
        }
    }
    block[set][way].lru = 0; // promote to the MRU position
}

void CACHE::replacement_final_stats()
{

}

#ifdef NO_CRC2_COMPILE
void InitReplacementState()
{
    
}

uint32_t GetVictimInSet (uint32_t cpu, uint32_t set, const BLOCK *current_set, uint64_t PC, uint64_t paddr, uint32_t type)
{
    return 0;
}

void UpdateReplacementState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t paddr, uint64_t PC, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    
}

void PrintStats_Heartbeat()
{
    
}

void PrintStats()
{

}
#endif
