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

/*uint32_t CACHE::lru_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
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
    uint32_t a,x=1,
             nway=NUM_WAY-1;
    if(NAME=="LLC"){
        if (way == NUM_WAY) {
            while(x==1){
                for (way=0; way<NUM_WAY; way++) {
                    if (block[set][way].lru == nway) {
                        
                        //cout << NAME << endl;
                        //evict_from_parent(block[set][way].address,instr_id,block[set][way].cpu);
                        
                        if(cpu==block[set][way].cpu){
                            a=evict_from_parent(block[set][way].address,instr_id,block[set][way].cpu);
                            cout << NAME << "   Same Cores!" << endl;
                            cout << a << endl << endl;
                            x=0;
                            break;
                        }
                        else{
                            a=evict_from_parent(block[set][way].address,instr_id,cpu);
                            cout << NAME << "   Different Cores!" << endl; 
                            cout << a << endl << endl;
                            x=1;
                            break;
                        }
                        
                        DP ( if (warmup_complete[cpu]) {
                        cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                        cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                        cout << dec << " lru: " << block[set][way].lru << endl; });
        
                        break;
                    }
                }
            }
        }
    }
    else{
        if (way == NUM_WAY) {
            for (way=0; way<NUM_WAY; way++) {
                if (block[set][way].lru == NUM_WAY-1) {
                    
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
}*/

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
    uint32_t j=NUM_WAY-1,x=1;
    if (way == NUM_WAY) {
        if(NAME=="LLC"){
            while(x==1){
                for (way=0; way<NUM_WAY; way++) {
                    if (block[set][way].lru == j) {
                        
                        if(cpu!=block[set][way].cpu){
                            //cout << "Block's CPU : " << block[set][way].cpu << "    Evicting CPU : " << cpu << "    Different Cores!   SPYYY   j : " << j << endl;
                            CROSS++;
                            //cout << "Cross Core Eviction!   " << CROSS << endl;
                            j--;
                            x=1;
                        }
                        else{
                            //cout << "Block's CPU : " << block[set][way].cpu << "    Evicting CPU : " << cpu << "    Same Core!    SAMEEE   j : " << j << endl;
                            x=0;
                            evict_from_parent(block[set][way].address,instr_id,block[set][way].cpu);
                        }
                        if(j==11){
                            //cout << "Breaking Point!    j : " << j << " Has a LRU : " << block[set][way].lru << endl;
                            evict_from_parent(block[set][way].address,instr_id,block[set][way].cpu);
                            x=0;
                        }
                        
    
                    DP ( if (warmup_complete[cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                    cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                    cout << dec << " lru: " << block[set][way].lru << endl; });
    
                    break;
                    }
                }
            }
        }
        else{
            for (way=0; way<NUM_WAY; way++) {
                if (block[set][way].lru == NUM_WAY-1) {
    
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
