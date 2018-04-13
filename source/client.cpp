#include <iostream>
#include "../external/spdlog/include/spdlog/spdlog.h"
#include <thread>

#include "../framework/RGBDRIClient.hpp"
#include "../framework/zhelpers.hpp"

/*
Logger setup
*/
// void logger_init(){
//   auto sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
//   auto logger = std::make_shared<spdlog::logger>("console",sink);
//   spdlog::register_logger(logger);
//   logger->set_level(spdlog::level::debug);
//   spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");
// }
//
// void client_init(int i){
//   auto context = std::make_shared<zmq::context_t>(4);
//   char j[10];
//
//   sprintf(j, "%d", i);
//   auto client = RGBDRIClient(context,j);
// }
//
// void client_init_and_play(int i){
//   auto context = std::make_shared<zmq::context_t>(4);
//   char j[10];
//
//   sprintf(j, "%d", i);
//   auto play_1 = Play(
//     "steppo.stream", //path to *.stream file that shall be played
//     true, //loop the stream
//     4, //number of rgbd sensors
//     24, //maximal frames per second
//     true, //rgbd *.stream file is compressed
//     10, //frame number to start from
//     100, //frame number to end on
//     /* optional */
//     "127.0.0.1:8001" //endpoint the rgbd file is streamed to (sync with ks file for demo)
//   );
//
//   auto client = RGBDRIClient(context,j);
//   client.execute(play_1);
//   sleep(within(100));
//   play_1.stop();
// }

int main(int argc, char const *argv[]) {
  // logger_init();
  // spdlog::get("console")->info("Start Application");
  //
  //
  // /* Single client test */
  // auto context = std::make_shared<zmq::context_t>(4);
  //
  // /* live rgbd stream */
  // // auto record_1 = Record(
  // //   "pupil1.stream", //path were the *.stream file is saved
  // //   "127.0.0.1:9000", //endpoint of the rgbd stream that shall be recorded
  // //   4, //number of rgbd sensors
  // //   30, //duration of the recording
  // //   true //rgbd *.stream file is compressed
  // // );
  //
  // auto play_1 = Play(
  //   "/tmp/pupil2.stream", //path to *.stream file that shall be played
  //   true, //loop the stream
  //   4, //number of rgbd sensors
  //   24, //maximal frames per second
  //   true, //rgbd *.stream file is compressed
  //   10, //frame number to start from
  //   100, //frame number to end on
  //   "127.0.0.1:7000" //endpoint the rgbd file is streamed to (sync with ks file for demo)
  // );
  //
  // auto play_3 = Play(
  //   "/opt/kinect-resources/rgbd-framework/rgbd-daemon/kinect_recordings/user_andre.stream", //path to *.stream file that shall be played
  //   true, //loop the stream
  //   4, //number of rgbd sensors
  //   24, //maximal frames per second
  //   true, //rgbd *.stream file is compressed
  //   10, //frame number to start from
  //   100, //frame number to end on
  //   "127.0.0.1:7002" //endpoint the rgbd file is streamed to (sync with ks file for demo)
  // );
  //
  // auto record_2 = Record(
  //   "/tmp/pupil2.stream", //path were the *.stream file is saved
  //   play_1.stream_endpoint, //endpoint of the rgbd stream that shall be recorded
  //   4, //number of rgbd sensors
  //   30, //duration of the recording
  //   true //rgbd *.stream file is compressed
  // );
  // //
  // // auto play_2 = Play(
  // //   "andre.stream", //path to *.stream file that shall be played
  // //   true, //loop the stream
  // //   4, //number of rgbd sensors
  // //   24, //maximal frames per second
  // //   true, //rgbd *.stream file is compressed
  // //   10, //frame number to start from
  // //   100 //frame number to end on
  // // );
  //
  //
  // char id[2] = "0";
  // auto client = RGBDRIClient(context,id);
  // client.execute(play_1);
  // //sleep(1);
  // //client.execute(record_2);
  // sleep(5);
  // //record_2.stop();
  // play_1.stop();
  // // client.execute(record_1);
  // // client.execute(record_2);
  // // client.execute(play_2);
  // //
  // // sleep(3);
  // //
  // // play_1.pause();
  // //
  // // sleep(3);
  // //
  // // play_1.resume();
  // //
  // // sleep(3);
  // // play_1.stop();
  //
  // /* Async clients test */
  // // std::vector<std::thread> clients;
  // //
  // // for (size_t i = 0; i < 100; i++) {
  // //   auto j = std::thread(client_init, i);
  // //   j.detach();
  // // }
  //
  // /* Async clients test with play execute */
  // // std::vector<std::thread> clients;
  // //
  // // for (size_t i = 0; i < 50; i++) {
  // //   auto j = std::thread(client_init_and_play, i);
  // //   j.detach();
  // // }
  //
  // while (true) {
  //   /* code */
  // }

  return 0;
}
