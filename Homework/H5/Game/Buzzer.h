#define NOTE_A4  440
#define NOTE_DS7 2489
#define NOTE_D8  4699
#define NOTE_G1  49
#define NOTE_GS2 104

const byte buzzerPin = A2;
const int menuSoundDuration = 100;
const int eatSoundDuration = 100;

bool muted = false;

void menuSound() {
  if (!muted) {
    tone(buzzerPin, NOTE_A4, menuSoundDuration);
  }
}

void eatSound() {
   if (!muted) {
    tone(buzzerPin, NOTE_G1, eatSoundDuration);
    tone(buzzerPin, NOTE_GS2, eatSoundDuration);
  }  
}