library ieee;
use ieee.std_logic_1164.all;

entity <<name>> is
    generic (
        RTSNOC_X_WIDTH          : integer   := 1;
        RTSNOC_Y_WIDTH          : integer   := 1;
        RTSNOC_H_WIDTH          : integer   := 3;
        RTSNOC_PAYLOAD_WIDTH    : integer   := 56;
        RMI_X_WIDTH             : integer   := 8;
        RMI_Y_WIDTH             : integer   := 8;
        RMI_H_WIDTH             : integer   := 8
    );
    port (
        clk                     : in    std_logic;
        rst_n                   : in    std_logic;
        rtsnoc_din              : out   std_logic_vector(2*(RTSNOC_X_WIDTH + RTSNOC_Y_WIDTH + RTSNOC_H_WIDTH) + RTSNOC_PAYLOAD_WIDTH - 1 downto 0);
        rtsnoc_wr               : out   std_logic;
        rtsnoc_wait             : in    std_logic;
        rtsnoc_dout             : in    std_logic_vector(2*(RTSNOC_X_WIDTH + RTSNOC_Y_WIDTH + RTSNOC_H_WIDTH) + RTSNOC_PAYLOAD_WIDTH - 1 downto 0);
        rtsnoc_rd               : out   std_logic;
        rtsnoc_nd               : in    std_logic;
        x_ori                   : in    std_logic_vector(RTSNOC_X_WIDTH - 1 downto 0);
        y_ori                   : in    std_logic_vector(RTSNOC_Y_WIDTH - 1 downto 0);
        h_ori                   : in    std_logic_vector(RTSNOC_H_WIDTH - 1 downto 0)
    );
end <<name>>;

architecture structural of <<name>> is
    -- 65      64      63...61 60      59       58...56 55...0
    -- x_ori   y_ori   h_ori   x_dst   y_dst    h_dst   payload
    constant RTSNOC_PAYLOAD_LSB : integer := 0;
    constant RTSNOC_H_ORI_LSB   : integer := RTSNOC_PAYLOAD_LSB + RTSNOC_PAYLOAD_WIDTH + RTSNOC_H_WIDTH + RTSNOC_Y_WIDTH + RTSNOC_X_WIDTH;
    constant RTSNOC_Y_ORI_LSB   : integer := RTSNOC_H_ORI_LSB + RTSNOC_H_WIDTH;
    constant RTSNOC_X_ORI_LSB   : integer := RTSNOC_Y_ORI_LSB + RTSNOC_Y_WIDTH;

    -- 79...72 71...64 63...56 55...32 31...0
    -- h       y       x       header  payload
    constant RMI_WIDTH          : integer := RMI_H_WIDTH + RMI_Y_WIDTH + RMI_X_WIDTH + RTSNOC_PAYLOAD_WIDTH;
    constant RMI_PAYLOAD_LSB    : integer := 0;
    constant RMI_X_LSB          : integer := RMI_PAYLOAD_LSB + RTSNOC_PAYLOAD_WIDTH;
    constant RMI_Y_LSB          : integer := RMI_X_LSB + RMI_X_WIDTH;
    constant RMI_H_LSB          : integer := RMI_Y_LSB + RMI_Y_WIDTH;

    signal rx_V_V_s : std_logic_vector(RMI_WIDTH - 1 downto 0);
    signal tx_V_V_s : std_logic_vector(RMI_WIDTH - 1 downto 0);

    signal ap_vld_s : std_logic;
    signal ap_ack_s : std_logic;

    signal din_x_dst_s      : std_logic_vector(RTSNOC_X_WIDTH - 1 downto 0);
    signal din_y_dst_s      : std_logic_vector(RTSNOC_Y_WIDTH - 1 downto 0);
    signal din_h_dst_s      : std_logic_vector(RTSNOC_H_WIDTH - 1 downto 0);
    signal din_payload_s    : std_logic_vector(RTSNOC_PAYLOAD_WIDTH - 1 downto 0);

    signal rx_h_s   : std_logic_vector(RMI_H_WIDTH - 1 downto 0);
    signal rx_y_s   : std_logic_vector(RMI_Y_WIDTH - 1 downto 0);
    signal rx_x_s   : std_logic_vector(RMI_X_WIDTH - 1 downto 0);
begin
    u_ap2rtsnoc : entity work.ap2rtsnoc
    port map (
        clk         => clk,
        rst_n       => rst_n,
        ap_vld      => ap_vld_s,
        ap_ack      => ap_ack_s,
        rtsnoc_wr   => rtsnoc_wr,
        rtsnoc_wait => rtsnoc_wait
    );

    u_<<name>>_top : entity work.<<name>>_top
    port map (
        ap_clk           => clk,
        ap_rst_n         => rst_n,
        rx_V_V           => rx_V_V_s,
        rx_V_V_ap_vld    => rtsnoc_nd,
        rx_V_V_ap_ack    => rtsnoc_rd,
        tx_V_V           => tx_V_V_s,
        tx_V_V_ap_vld    => ap_vld_s,
        tx_V_V_ap_ack    => ap_ack_s
    );

    -- ap -> rtsnoc
    din_x_dst_s     <= tx_V_V_s(RMI_X_LSB + RTSNOC_X_WIDTH - 1 downto RMI_X_LSB);
    din_y_dst_s     <= tx_V_V_s(RMI_Y_LSB + RTSNOC_Y_WIDTH - 1 downto RMI_Y_LSB);
    din_h_dst_s     <= tx_V_V_s(RMI_H_LSB + RTSNOC_H_WIDTH - 1 downto RMI_H_LSB);
    din_payload_s   <= tx_V_V_s(RMI_PAYLOAD_LSB + RTSNOC_PAYLOAD_WIDTH - 1 downto RMI_PAYLOAD_LSB);
    rtsnoc_din      <= x_ori & y_ori & h_ori & din_x_dst_s & din_y_dst_s & din_h_dst_s & din_payload_s;

    -- rtsnoc -> ap
    rx_h_s <= (rx_h_s'length - 1 downto RTSNOC_H_WIDTH => '0') & rtsnoc_dout(RTSNOC_H_ORI_LSB + RTSNOC_H_WIDTH - 1 downto RTSNOC_H_ORI_LSB);
    rx_y_s <= (rx_y_s'length - 1 downto RTSNOC_Y_WIDTH => '0') & rtsnoc_dout(RTSNOC_Y_ORI_LSB + RTSNOC_Y_WIDTH - 1 downto RTSNOC_Y_ORI_LSB);
    rx_x_s <= (rx_x_s'length - 1 downto RTSNOC_X_WIDTH => '0') & rtsnoc_dout(RTSNOC_X_ORI_LSB + RTSNOC_X_WIDTH - 1 downto RTSNOC_X_ORI_LSB);
    rx_V_V_s <= rx_h_s & rx_y_s & rx_x_s & rtsnoc_dout(RTSNOC_PAYLOAD_LSB + RTSNOC_PAYLOAD_WIDTH - 1 downto RTSNOC_PAYLOAD_LSB);
end architecture structural;
