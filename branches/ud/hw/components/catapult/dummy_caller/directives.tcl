directive set /$toplevel/core/agent._buffer:rsc -MAP_TO_MODULE {[Register]}
directive set /$toplevel/core/agent.dispatch:case-18:t1.data:rsc -MAP_TO_MODULE {[Register]}
directive set /$toplevel/core/agent.dispatch:case-16:t0.data:rsc -MAP_TO_MODULE {[Register]}

directive set /Dummy_Caller_Node/core/agent.rsp_tem_tudo_faz_tudo:dec_pkt.data#19:rsc -MAP_TO_MODULE {[Register]}
directive set /Dummy_Caller_Node/core/agent.dispatch:case-19:t1.data:rsc -MAP_TO_MODULE {[Register]}
directive set /Dummy_Caller_Node/core/agent.__aes._cipher_key:rsc -MAP_TO_MODULE {[Register]}
directive set /Dummy_Caller_Node/core/agent.__aes._addRoundKey#59:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#39:for#2:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#39:for#2 -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#39:for#1:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#39:for#1 -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#39:for:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#39:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._subBytes#39:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._addRoundKey#58:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._mixColumns#19:for:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._mixColumns#19:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#38:for#2:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#38:for#2 -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#38:for#1:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#38:for#1 -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#38:for:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._shiftRows#38:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._subBytes#38:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._cipher#19:for -PIPELINE_INIT_INTERVAL 1
directive set /Dummy_Caller_Node/core/agent.__aes._addRoundKey#57:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes.add_key#19:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._expandKey#19:while -PIPELINE_INIT_INTERVAL 1
directive set /Dummy_Caller_Node/core/agent.__aes._expandKey#19:while:for#1 -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._scheduleCore#19:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._rotate#19:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._expandKey#19:while:for -UNROLL yes
directive set /Dummy_Caller_Node/core/agent.__aes._expandKey#19:while -UNROLL no
directive set /Dummy_Caller_Node/core/agent.__aes._expandKey#19:while -ITERATIONS 40




