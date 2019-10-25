//
// Created by panda on 10/17/19.
//

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <storage/storage.hpp>
#include <tgbot/tgbot.h>
#include <threadsafequeue.hpp>

using namespace TgBot;

int main() {
  std::string token(getenv("TOKEN"));
  printf("Token: %s\n", token.c_str());
  auto st = new storage("db");
  ThreadSafeQueue<LogData *> msg_q;
  std::mutex logging_mut;
  std::condition_variable cv;
  auto logger_thread = std::thread([&st, &msg_q, &cv, &logging_mut]() {
    while (true) {
      std::unique_lock<std::mutex> lk(logging_mut);
      while (msg_q.isEmpty()) {
        cv.wait(lk);
      }
      auto logData = msg_q.pop();
      st->writeLog(logData->user_id, logData->msg);
      delete logData;
    }
  });

  Bot bot(token);
  bot.getEvents().onCommand("start", [&bot, &st](Message::Ptr message) {
    printf("start chat id = %ld", message->chat->id);
    std::stringstream response;
    response << "Hi! ";
    if (st->isUserExists(message->from->id)) {
      response << "I am already know you. You are ";
      auto username = st->getUserName(message->from->id);
      response << username;
    } else {
      response
          << "I am sorry, but I do not know you yet and there is no way you can introduce yourself to me without contacting real me!";
    }
    bot.getApi().sendMessage(message->chat->id, response.str());
  });
  bot.getEvents().onCommand("list", [&bot, &st](Message::Ptr message) {
    if (st->isUserExists(message->from->id)) {
      auto c = st->getCommunicationList(message->from->id);
      std::stringstream response;
      response << "Send me without quotes '/i code-word' to get text or '/v code-word' to get voice where ";
      for (const auto &communice: c) {
        response << "\ncode-word=" << communice.first << " to get from me " << communice.second;
      }
      bot.getApi().sendMessage(message->chat->id, response.str());
    } else {
      bot.getApi().sendMessage(message->chat->id,
                               "I am sorry, but I do not know you yet and there is no way you can introduce yourself to me without contacting real me!");
    }
  });

  bot.getEvents().onCommand("i", [&bot, &st](Message::Ptr message) {
    if (message->text.length() == 2) {
      auto username = st->getUserName(message->from->id);
      bot.getApi().sendMessage(message->chat->id, username +
                                                  ", you have missed code-word. You need to write '/i' and then code-word in english as it follows by '/list'");
      return;
    }
    std::string category = message->text.substr(3, message->text.length());
    auto response = st->getCommunice(message->from->id, category);
    bot.getApi().sendMessage(message->chat->id, response);
  });

  bot.getEvents().onCommand("v", [&bot, &st](Message::Ptr message) {
    if (message->text.length() == 2) {
      auto username = st->getUserName(message->from->id);
      bot.getApi().sendMessage(message->chat->id, username +
                                                  ", you have missed code-word. You need to write '/v' and then code-word in english as it follows by '/list'");
      return;
    }
    std::string category = message->text.substr(3, message->text.length());
    auto filename = st->getCommuniceId(message->from->id, category);
    bot.getApi().sendAudio(message->chat->id,
                           InputFile::fromFile("voice/" + category + "/" + std::to_string(filename) + ".m4a", "audio"));
  });


  bot.getEvents().onNonCommandMessage([&bot, &st, &msg_q, &cv](Message::Ptr message) {
    printf("User wrote %s\n", message->text.c_str());
    std::stringstream response;
    if (st->isUserExists(message->from->id)) {
      auto username = st->getUserName(message->from->id);
      response << username << "! To see what kind of communication you can get from me send me '/list' without quotes";
    }
    bot.getApi().sendMessage(message->chat->id, response.str());
    msg_q.push(new LogData{message->text, message->from->id});
    cv.notify_all();
  });

  signal(SIGINT, [](int s) {
    printf("SIGINT got\n");
    exit(0);
  });

  try {
    printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    bot.getApi().deleteWebhook();

    TgLongPoll longPoll(bot);
    while (true) {
      printf("Long poll started\n");
      longPoll.start();
    }
  } catch (std::exception &e) {
    printf("error: %s\n", e.what());
  }

  return 0;
}