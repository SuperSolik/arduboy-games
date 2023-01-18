#include <Arduboy2.h>


template<typename T>
struct Point2D {
    Point2D() : Point2D(0, 0) {}

    Point2D(T x, T y) {
        this->x = x;
        this->y = y;
    };

    Point2D translated(T dx, T dy) {
      return Point2D(this->x + dx, this->y + dy);
    }

    Point2D mul(T dx, T dy) {
      return Point2D(this->x * dx, this->y * dy);
    }

    template<typename V>
    Point2D<V> as_type(){
      return Point2D<V>(static_cast<V>(this->x), static_cast<V>(this->y));
    }
    
    T x;
    T y;
};


template<typename T>
struct Point3D {
    Point3D() : Point3D(0, 0, 0) {}

    Point3D(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    };

    Point2D<float> to2D(float half_angle) {
        float depth = tan(half_angle) * this->z;

        return Point2D<float>(
            (float)this->x / depth, 
            (float)this->y / depth
        );
    }

     Point3D rotateX(float angle) {
      return Point3D(
        this->x,
        this->y * cos(angle) - this->z * sin(angle),
        this->y * sin(angle) + this->z * cos(angle)
      );
    }

    Point3D rotateY(float angle) {
      return Point3D(
        this->x * cos(angle) - this->z * sin(angle),
        this->y,
        this->x * sin(angle) + this->z * cos(angle)
      );
    }

    Point3D translated(T dx, T dy, T dz) {
      return Point3D(this->x + dx, this->y + dy, this->z + dz);
    }

    Point3D mul(T dx, T dy, T dz) {
      return Point3D(this->x * dx, this->y * dy, this->z * dz);
    }

    T x;
    T y;
    T z;
};

class Game {
 public:
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr float degrees_to_radians(float degrees) {
    return degrees / 180 * M_PI;
  }

  Game(const Arduboy2& arduboy, uint8_t fps = DEFAULT_FPS) {
    this->arduboy = arduboy;
    this->fps = fps;
  }

  void init() {
    this->arduboy.begin();
    this->arduboy.setFrameRate(this->fps);
    this->arduboy.initRandomSeed();
    this->reset();
  }

  void run() {
    if (!this->arduboy.nextFrame()) return;
    this->arduboy.clear();
    this->arduboy.pollButtons();

    uint64_t elapsed_time = millis() - current_time;

    this->player_input(elapsed_time / (float)1000);
    this->render(elapsed_time / (float)1000);

    current_time += elapsed_time;

    this->arduboy.display();
  }

 private:
  void reset() {
    current_time = millis();
  }

  void player_input(float elapsed_time) {
    if (this->arduboy.pressed(LEFT_BUTTON)) {
      rotate_angle_y = (rotate_angle_y - 5) % 360;
    }

    if (this->arduboy.pressed(RIGHT_BUTTON)) {
      rotate_angle_y = (rotate_angle_y + 5) % 360;
    }

    if (this->arduboy.pressed(DOWN_BUTTON)) {
      rotate_angle_x = (rotate_angle_x + 5) % 360;
    }

    if (this->arduboy.pressed(UP_BUTTON)) {
      rotate_angle_x = (rotate_angle_x - 5) % 360;
    }
  }

  void render(float elapsed_time) {
    Point2D<int16_t> transformed_front[4];
    Point2D<int16_t> transformed_back[4];

    float r_x = Game::degrees_to_radians(this->rotate_angle_x);
    float r_y = Game::degrees_to_radians(this->rotate_angle_y);
    float fov_2_r = Game::degrees_to_radians(half_fov);

    for(uint8_t i = 0; i < 4; i ++) {
      transformed_front[i] = this->front[i].rotateX(r_x).rotateY(r_y).translated(0, 0, 5).to2D(
          fov_2_r
        ).mul(WIDTH, 2*HEIGHT).translated(WIDTH / 2, HEIGHT / 2).as_type<int16_t>();
    }

    for(uint8_t i = 0; i < 4; i ++) {
      transformed_back[i] = this->back[i].rotateX(r_x).rotateY(r_y).translated(0, 0, 5).to2D(
          fov_2_r
        ).mul(WIDTH, 2*HEIGHT).translated(WIDTH / 2, HEIGHT / 2).as_type<int16_t>();
    }

    for(uint8_t i = 0; i < 4; i ++) {
      this->draw_line(
        transformed_front[i],
        transformed_front[(i  + 1) % 4]
      );
    }

    for(uint8_t i = 0; i < 4; i ++) {
      this->draw_line(
        transformed_back[i],
        transformed_back[(i  + 1) % 4]
      );
    }

    for(uint8_t i = 0; i < 4; i ++) {
      this->draw_line(
        transformed_front[i],
        transformed_back[i]
      );
    }
  }

  void draw_line(const Point2D<int16_t>& p1, const Point2D<int16_t>& p2){
    this->arduboy.drawLine(p1.x, p1.y, p2.x, p2.y);
  }

  Arduboy2 arduboy;
  uint8_t fps;

  uint64_t current_time = 0;

  int16_t rotate_angle_y = 0;
  int16_t rotate_angle_x = 0;

  float fov = 90;
  float half_fov = fov / 2;

  Point3D<float> front[4] = {
    Point3D<float>(-0.5, -0.5, -0.5),
    Point3D<float>(-0.5, 0.5, -0.5),
    Point3D<float>(0.5, 0.5, -0.5),
    Point3D<float>(0.5, -0.5, -0.5),
  };

  Point3D<float> back[4] = {
    front[0].translated(0, 0, 1),
    front[1].translated(0, 0, 1),
    front[2].translated(0, 0, 1),
    front[3].translated(0, 0, 1)
  };
};