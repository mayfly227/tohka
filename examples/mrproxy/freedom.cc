//
// Created by li on 2022/4/22.
//

#include "freedom.h"

#include "tohka/ioloop.h"

void FreeDom::on_connection(const TcpEventPrt_t& conn) {
  if (conn->Connected()) {
    assert(ctx_);
    log_warn("FreeDom connected");
    conn->SetTcpNoDelay();
    // 注册conn到ctx中
    ctx_->out = conn;
    //    ctx_->out_handler = shared_from_this();

    if (ctx_->in) {
      // in结点开启读事件
      ctx_->in->StartReading();
      if (ctx_->in->GetInputBuf()->GetReadableSize() > 0) {
        Process(ctx_);
      }
    }
  } else {
    log_warn("FreeDom disconnected");
    client_->SetOnConnection(DefaultOnConnection);
    client_->SetOnMessage(DefaultOnMessage);
    // 关闭in结点
    // BUG
    // 释放ctx资源,因为ctx持有当前对象的共享指针
    if (ctx_) {
      if (ctx_->in) {
        ctx_->in->ShutDown();
      }
      log_warn("FreeDom on DisConnected reset ctx");
      ctx_.reset();
    }
  }
}
void FreeDom::on_recv(const TcpEventPrt_t& conn, IoBuf* buf) {
  // HINT 调用in结点来处理数据
  if (!ctx_) {
    log_fatal("FreeDom::on_recv");
  }
  assert(ctx_);
  ctx_->in_handler->Process(ctx_);
}
void FreeDom::StartClient() {
  auto self(shared_from_this());

  // 如果连不上就让client一直连，直到in结点主动关闭
  client_->SetOnConnection(
      [this, self](const TcpEventPrt_t& conn) { on_connection(conn); });
  client_->SetOnMessage([this, self](const TcpEventPrt_t& conn, IoBuf* buf) {
    on_recv(conn, buf);
  });
  // client_->SetOnTimeOut([this, self] {
  //   log_warn("SetOnTimeOut");
  //   // ctx可能为空
  //   // 这个时候freedom对象已经释放了,但是这个是client一定没有连接上才会调用
  //   if (ctx_) {
  //     log_fatal("not ctx");
  //     // 关闭in结点
  //     if (ctx_->out) {
  //       return;
  //     }
  //     // 解决这个时候in已经关闭了连接
  //     ctx_->in->StartReading();

  //     ctx_->in->ShutDown();
  //     client_->SetOnConnection(DefaultOnConnection);
  //     client_->SetOnMessage(DefaultOnMessage);
  //     client_->Stop();
  //     ctx_.reset();
  //   }
  // });
  client_->Connect();
}

void FreeDom::Process(const ContextPtr_t& ctx) {
  // 表示in结点已经收到了数据
  // 数据交给本类处理
  // BUG ctx还在？
  if (!ctx) {
    log_fatal("no ctx");
  }
  assert(ctx);
  assert(ctx_);
  assert(ctx_->in == ctx->in);
  assert(ctx_->out == ctx->out);
  if (ctx->in && ctx->out) {
    IoBuf* buf_in = ctx->in->GetInputBuf();
    ctx->out->Send(buf_in);
  } else {
    log_warn("in is release dont send");
    exit(-2);
  }
}

FreeDom::FreeDom(const ContextPtr_t& ctx) {
  ctx_ = ctx;
  NetAddress addr = ctx->addr;
  client_ = std::make_unique<TcpClient>(IoLoop::GetLoop(), addr, "None");
}
void FreeDom::DisConnected() {
  if (ctx_) {
    // 如果没有连接上，那么就释放ctx,以释放FreeDom的内存
    if (!ctx_->out) {
      ctx_.reset();
      log_warn("DisConnected reset ctx");
      client_->SetOnConnection(DefaultOnConnection);
      client_->SetOnMessage(DefaultOnMessage);
      client_->Stop();
    } else {
      client_->Disconnect();
    }
  }
}
FreeDom::~FreeDom() { log_fatal("~FreeDom"); }
// OutHandler* FreeDomFactory::Create(Point* point) { return new FreeDom(); }
// void FreeDomFactory::Init() { RegOut("freedom", new FreeDomFactory()); }
