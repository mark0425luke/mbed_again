#ifndef HAHACONFIG_H_
#define HAHACONFIG_H_

// The number of labels (without negative)
#define label_num 3

struct Config {

  // This must be the same as seq_length in the src/model_train/config.py
  //const int seq_length = 64;
  int seq_length = 64;

  // The number of expected consecutive inferences for each gesture type.
  const int consecutiveInferenceThresholds[label_num] = {20, 5, 5};//ring is more sensitive, can be 20, slope and left right can be less

  const char* output_message[label_num] = {
        "RING:\n\r"
        "          *       \n\r"
        "       *     *    \n\r"
        "     *         *  \n\r"
        "    *           * \n\r"
        "     *         *  \n\r"
        "       *     *    \n\r"
        "          *       \n\r",
        "SLOPE:\n\r"
        "        *        \n\r"
        "       *         \n\r"
        "      *          \n\r"
        "     *           \n\r"
        "    *            \n\r"
        "   *             \n\r"
        "  *              \n\r"
        " * * * * * * * * \n\r",
	   "left right:\n\r"
      " * * * * * * * * \n\r"
      " * 		  \n\r"
      " * * * * * * * * \n\r"};
    
};

Config config;

#endif // HAHACONFIG_H_