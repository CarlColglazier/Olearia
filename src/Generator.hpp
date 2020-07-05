class Generator {
public:
	virtual void Init(float sample_rate) = 0;
	virtual void Control(float f) = 0;
	virtual float Process(float f) = 0;
};
