;WSRData 2 1


send data_t "<data_t>"

send  connect "messagenum"

send bgw_RelayRegiRequest "<bgw_RelayRegiRequest>"

send fgw_RelayRegiRequest "<fgw_RelayRegiRequest>"

recv buf1 12


send bgw_RelayQueryConnectionR "<bgw_RelayQueryConnectionR>"

send bgw_RelayRegisterRequest "<bgw_RelayRegisterRequest>"


send fgw_RelayQueryConnectionRe "<fgw_RelayQueryConnectionRe>"
recv  buf2 20

send fgw_RelayRegiRequest "<fgw_RelayRegiRequest>"

send fgw_RelayMsgConnectionReq "<fgw_RelayMsgConnectionReq>"
recv buf_RelayMsgConnectionReq 12

send bgw_RelayMsgConnectionRes "<bgw_RelayMsgConnectionRes>"
recv buf_RelayMsgConnectionRes 12

send fgw_RelayDataIndication "<fgw_RelayDataIndication>"
recv buf_RelayDataIndication 14

recv get_data 123


-1
