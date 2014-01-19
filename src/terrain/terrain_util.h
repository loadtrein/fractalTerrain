namespace octet {
  
 class Point{
 
    float x;
    float y;
    float z;
    bool justGenerated;

 public:

    Point(){
      this->x = 0.0;
      this->y = 0.0;
      this->z = 0.0;
      this->justGenerated = false;
    }

    Point(float x_,float y_,float z_, bool b){
      this->x = x_;
      this->y = y_;
      this->z = z_;
      this->justGenerated = b;
    }

    float getX(){
      return this->x;
    }

    void setX(float x){
      this->x = x; 
    }

    float getY(){
      return this->y;
    }

    void setY(float y){
      this->y = y; 
    }

    float getZ(){
      return this->z;
    }

    void setZ(float z){
      this->z = z; 
    }

    bool isJustGenerated(){
      return this->justGenerated;
    }

    void setJustGenerated(bool b){
      this->justGenerated = b;
    }
  };


  class Tile {
  public:
    Point points [4];
  };
}