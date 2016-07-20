#include <cpu.h>
#include <utility/ostream.h>
#include <utility/assert_aux.h>
#include <architecture/ia32/mmu_aux.h>

using namespace EPOS;

typedef CPU::Log_Addr Log_Addr;
typedef CPU::Phy_Addr Phy_Addr;
typedef MMU::Page_Directory Page_Directory;
typedef MMU::Page_Table Page_Table;
typedef MMU::PT_Entry PT_Entry;

static const unsigned int PAGE_SIZE = 4096;

OStream cout;

void test_mmu_page_table_remap();

int main()
{
    cout << "MMU_Aux Test." << endl;

    if (Traits<Build>::MODE == Traits<Build>::KERNEL) {
        MMU_Aux::check_memory_mapping();
    }

    // MMU_Aux::dump_memory_mapping();

    test_mmu_page_table_remap();

    cout << "Bye." << endl;
}


void test_mmu_page_table_remap()
{
    volatile unsigned long * alpha = new unsigned long[4];
    while (reinterpret_cast<unsigned long>(alpha) != (reinterpret_cast<unsigned long>(alpha) & 0xfffff000)) {
        alpha = new unsigned long[4];
    }

    volatile  unsigned long * beta = new unsigned long[4];
    while (reinterpret_cast<unsigned long>(beta) != (reinterpret_cast<unsigned long>(beta) & 0xfffff000)) {
        beta = new unsigned long[4];
    }

    alpha[0] = 0xdeadbeef;
    beta[0] = 0xfeedabee;

    cout << "alpha = " << (void *) alpha << endl;
    cout << "beta = " << (void *) beta << endl;
    cout << "alpha[0] = " << reinterpret_cast<void *>(alpha[0]) << endl;
    cout << "beta[0] = " << reinterpret_cast<void *>(beta[0]) << endl;

    unsigned long page_frame_present = 0;
    Phy_Addr alpha_phy_addr = Phy_Addr(MMU_Aux::physical_address(reinterpret_cast<unsigned long>(alpha), &page_frame_present));
    Phy_Addr beta_phy_addr = Phy_Addr(MMU_Aux::physical_address(reinterpret_cast<unsigned long>(beta), &page_frame_present));
    cout << "alpha_phy_addr = " << alpha_phy_addr << endl;
    cout << "beta_phy_addr = " << beta_phy_addr << endl;


    assert_equals(alpha[0], static_cast<unsigned long>(0xdeadbeef));
    assert_equals(beta[0], static_cast<unsigned long>(0xfeedabee));

    Log_Addr alpha_log_addr = Log_Addr(alpha);
    Log_Addr beta_log_addr = Log_Addr(beta);
    Page_Directory * page_directory = MMU::current();
    cout << "page_directory = " << reinterpret_cast<void *>(page_directory) << endl;

    unsigned int alpha_page_table_index = MMU::directory(alpha_log_addr);
    cout << "alpha_page_table_index = " << alpha_page_table_index << endl;
    Page_Table * alpha_page_table = MMU::discard_flags((*page_directory)[alpha_page_table_index]);
    cout << "alpha_page_table = " << reinterpret_cast<void *>(alpha_page_table) << endl;

    unsigned int alpha_page_index = MMU::page(alpha_log_addr);
    ASM("pre_alpha_remap:");
    alpha_page_table->remap(MMU::discard_page_offset(beta_phy_addr), alpha_page_index, alpha_page_index + 1, MMU::IA32_Flags::APP_CODE); // Just for fun, making the page read-only
    ASM("pos_alpha_remap:");
    MMU::flush_tlb();

    unsigned int beta_page_table_index = MMU::directory(beta_log_addr);
    cout << "beta_page_table_index = " << beta_page_table_index << endl;
    Page_Table * beta_page_table = MMU::discard_flags((*page_directory)[beta_page_table_index]);
    cout << "beta_page_table = " << reinterpret_cast<void *>(beta_page_table) << endl;
    unsigned int beta_page_index = MMU::page(beta_log_addr);
    ASM("pre_beta_remap:");
    beta_page_table->remap(MMU::discard_page_offset(alpha_phy_addr), beta_page_index, beta_page_index + 1, MMU::IA32_Flags::APP_CODE); // Just for fun, making the page read-only
    ASM("pos_beta_remap:");
    MMU::flush_tlb();

    Phy_Addr new_alpha_phy_addr = Phy_Addr(MMU_Aux::physical_address(reinterpret_cast<unsigned long>(alpha), &page_frame_present));
    Phy_Addr new_beta_phy_addr = Phy_Addr(MMU_Aux::physical_address(reinterpret_cast<unsigned long>(beta), &page_frame_present));
    cout << "new_alpha_phy_addr = " << new_alpha_phy_addr << endl;
    cout << "new_beta_phy_addr = " << new_beta_phy_addr << endl;
    cout << "alpha = " << (void *) alpha << endl;
    cout << "beta = " << (void *) beta << endl;
    cout << "alpha[0] = " << reinterpret_cast<void *>(alpha[0]) << endl;
    cout << "beta[0] = " << reinterpret_cast<void *>(beta[0]) << endl;


    assert_equals(new_alpha_phy_addr == beta_phy_addr, true);
    cout << "Assertion: new_alpha_phy_addr == beta_phy_addr ... hold" << endl;
    assert_equals(new_beta_phy_addr == alpha_phy_addr, true);
    cout << "Assertion: new_beta_phy_addr == alpha_phy_addr ... hold" << endl;
    assert_equals(alpha[0], static_cast<unsigned long>(0xfeedabee));
    assert_equals(beta[0], static_cast<unsigned long>(0xdeadbeef));
}
