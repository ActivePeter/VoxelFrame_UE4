use crate::base::*;

const REC_HEAD_SIZE: u8 = 5;

struct ReceiveHandler {
    head_rec_cnt: u8,
    head_buff: [u8; 5],
    pack_len: u32,
    pack_id: u8,
    body_buff: Vec<u8>,
    body_rec_cnt: u32,
}

impl ReceiveHandler {
    //处理流式接收内容，解决粘包。半包
    pub fn handle_rec(&mut self, rec_buf: &[u8], _byte_cnt: usize) {
        let mut handled_offset = 0;
        while handled_offset < _byte_cnt {
            //剩余的字符数
            let byte_cnt_left = _byte_cnt - handled_offset;
            if (self.head_rec_cnt < REC_HEAD_SIZE)
            {
                //头本次还是未收全
                if (byte_cnt_left + self.head_rec_cnt < REC_HEAD_SIZE as usize)
                {
                    for i in 0..byte_cnt_left - 1
                    {
                        self.head_buff[self.head_rec_cnt + i] = rec_buf[handled_offset + i];
                    }
                    self.head_rec_cnt += byte_cnt_left;

                    return;//直接结束
                } else {//本次收全
                    for i in 0..REC_HEAD_SIZE - self.head_rec_cnt
                    {
                        self.head_buff[self.head_rec_cnt + i] = rec_buf[handled_offset + i];
                    }
                    handled_offset += REC_HEAD_SIZE - self.head_rec_cnt;

                    self.calc_buf_2_pack_head();

                    //memcpy(data_handle_record.get_head_write_pos(),
                    //	received_data.data(),
                    //	//剩余大小
                    //	DataHandleRecord::Data_Head_Size - data_handle_record.head_rec_cnt);
                    self.head_rec_cnt = REC_HEAD_SIZE;

                    self.update_body_buff_size();
                    //准备好vector空间
                    continue;
                }
            }

            //处理body pack
            // 1.剩余数据小于需要读的包数量(不够
            if (byte_cnt_left < (self.pack_len -
                self.body_rec_cnt) as usize)
            {
                //VF_LogWarning("handle case 3");
                //memcpy(data_handle_record,)
                self.write_data_2_body(received_data.data() + handled_offset, byte_cnt_left);
                //handled_offset += byte_cnt_left;
                return true;
            } else
            //完成读包
            {
                auto
                len = data_handle_record.pack_head.pack_len -
                    data_handle_record.body_rec_cnt;
                //VF_LogWarning("handle case 4 %d %d", len, data_handle_record.pack_head.pack_len);
                data_handle_record.write_data_2_body(received_data.data() + handled_offset, len);
                handled_offset += len;

                //处理包
                // ...
                _HandlePack::handle(
                    data_handle_record.pack_head.pack_id,
                    data_handle_record.body_buff,
                    data_handle_record.pack_head.pack_len);
                //GameContext::ecs
                //重置状态
                //VF_LogWarning("end one pack %d %d %d", data_handle_record.body_rec_cnt, data_handle_record.head_rec_cnt, total++);
                data_handle_record.reset();
                //VF_LogWarning("end one pack %d %d", data_handle_record.body_rec_cnt, data_handle_record.head_rec_cnt);
                /*while (1)
                {
                }*/
            }
        }
    }
    fn calc_buf_2_pack_head(&mut self) {
        self.pack_id = head_buff[0];
        self.pack_len =
            (head_buff[1] as u32) << 24 |
                (head_buff[2] as u32) << 16 |
                (head_buff[3] as u32) << 8 |
                (head_buff[4] as u32);
    }
    fn update_body_buff_size(&mut self) {
        if (self.body_buff.size() < self.pack_len)
        {
            self.body_buff.resize(self.pack_len as usize, 0);
        }
    }
    fn write_data_2_body(&mut self, slice: &[u8]) {}
}