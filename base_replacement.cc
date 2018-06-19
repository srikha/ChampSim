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
    //uint32_t b=block[set][way].lru;

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

    uint32_t a[4], // Array storing blocks present in private cache of victim
             nway=NUM_WAY-1, // LRU 
             wayf=0, 
             max=NUM_WAY<4?NUM_WAY-3:NUM_WAY-5; // Max limit till lru-4
    // LRU victim
    if (way == NUM_WAY) {
        while(!wayf) { // Checks whether the way is found
            for (way=0; way<NUM_WAY; way++) {
                if (block[set][way].lru == nway) {
                    
                    if (block[set][way].cpu == cpu) { // Same cpu trying to evict the block
                        cout << "Requesting CPU: " << cpu << "    Evicted block's CPU: " << block[set][way].cpu << " LRU : " << block[set][way].lru << endl;
                        
                        
                        wayf=1; // Found the block to be evicted
                        break;
                    }
                    
                    else { // No Eviction of block if owner of block isn't evicting
                        a[NUM_WAY-nway-1]=way; // Stores the blocks that are present in private cache of victim
                        wayf=0; // To check for a block in next iteration
                        break;
                    }

            
                            DP ( if (warmup_complete[cpu]) {
                            cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                            cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                            cout << dec << " lru: " << block[set][way].lru << endl; });
            
                            //break;
                     
                }
            }
            nway--; // To check next block to evict
            if (nway==max) // First 4 lru blocks are present in private cache
            {
                wayf=1; // Found the block to be evicted
                way=a[0]; // Evicts the First of the four blocks
                cout << " Random Eviction! LRU of Evicted Block : " << block[set][way].lru << endl;
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
