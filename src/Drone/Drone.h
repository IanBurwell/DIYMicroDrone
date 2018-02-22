

class Drone {
  struct Parameters {

    short hz;
  };

  public:
    Drone();
    void updateRun();

    void saveParams();
    Parameters params;


  private:

};
