#include <tstp.h>
#include <utility/random.h>
#include <utility/math.h>

__BEGIN_SYS

SGGR::Time SGGR::backoff(char * message) {
    auto h = reinterpret_cast<TSTP_API::Header*>(message);
    if(h->origin_address() == _my_address) {
        return random_backoff();
    } else {
        Local_Address dst;
        if(h->message_type() == TSTP_API::MESSAGE_TYPE::DATA) {
            if(Traits<TSTP>::is_sink) {
                return random_backoff();
            }
            dst = _sink_address;
        } else if(h->message_type() == TSTP_API::MESSAGE_TYPE::INTEREST) {
            auto remote_addr = reinterpret_cast<TSTP_API::Interest_Message*>(message)->destination();
            if(accept(remote_addr)) {
                return random_backoff();
            }
            dst = remote_addr.center;
        } // TODO: BOOTSTRAP
        auto D = _my_address - dst;
        auto Dmsg = h->last_hop_address() - dst;
        auto R = MAC_Config::RADIO_RADIUS;
        auto g = MAC_Config::G;
        auto S = MAC_Config::SLEEP_PERIOD;
        auto ret = ((Math::abs(D - (Dmsg - R))*S) / (g*R)) * g;
        return ret;
    }
}

bool SGGR::should_forward(const char * data, const Remote_Address & dst) {
    auto header = reinterpret_cast<const TSTP_API::Header *>(data);
    auto src = header->last_hop_address();
    auto dest = dst.center;
    auto my_distance = _my_address - dest;
    auto their_distance = src - dest;
    return my_distance < their_distance;
}

bool SGGR::should_forward(const char * data, const Local_Address & dst) {
    auto header = reinterpret_cast<const TSTP_API::Header *>(data);
    auto src = header->last_hop_address();
    auto dest = dst;
    auto my_distance = _my_address - dest;
    auto their_distance = src - dest;
    return my_distance < their_distance;
}


__END_SYS
