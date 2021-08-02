#pragma once


#include "glm.h"
#include <minmax.h>
#include <vector>





struct Lerper
{
public:
	glm::vec3 start, end;
	float step = 0;
	float t = 0;
	float wait = 0;
	bool finished = 0;

	Lerper() {}

	Lerper(glm::vec3 start, glm::vec3 end, float step)
	{
		this->start = start;
		this->end = end;
		this->step = step;
	}
	Lerper(glm::vec3 start, glm::vec3 end, float step, float wait)
	{
		this->start = start;
		this->end = end;
		this->step = step;
		this->wait = wait;
	}

	void reset()
	{
		finished = 0;
		t = 0;
	}

	bool isFinished()
	{
		return finished;
	}

	glm::vec3 lerpStepSmooth(float step)
	{
		if (t < 1)
		{
			t += step;
			return start + (end - start) * smoothstep(0, 1, t);
		}
		stepWait(step);
		return end;
	}

	glm::vec3 lerpStep(float step)
	{
		if (t < 1)
		{
			t += step;
			return start + (end - start) * t;
		}
		stepWait(step);
		return end;
	}

	glm::vec3 lerpStepSmooth()
	{
		return lerpStepSmooth(step);
	}

	glm::vec3 lerpStep()
	{
		return lerpStep(step);
	}

	glm::vec3 getStart()
	{
		return start;
	}

	glm::vec3 getEnd()
	{
		return end;
	}

	float getStep()
	{
		return step;
	}

	float getWait()
	{
		return wait;
	}

	float clamp(float x, float lowerlimit, float upperlimit) {
		if (x < lowerlimit)
			x = lowerlimit;
		if (x > upperlimit)
			x = upperlimit;
		return x;
	}
	float smoothstep(float edge0, float edge1, float x) {
		// Scale, bias and saturate x to 0..1 range
		x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
		// Evaluate polynomial
		return x * x * (3 - 2 * x);
	}
	
	void stepWait(float step)
	{
		if (t < 1 + wait)
			t += step;
		else
		{
			t = 1 + wait;
			finished = 1;
		}		
	}
};

struct BezierLerper : Lerper
{
	glm::vec3 c1, c2;

	BezierLerper() {}

	BezierLerper(glm::vec3 start, glm::vec3 end, glm::vec3 c1, glm::vec3 c2, float step)
	{
		this->start = start;
		this->c1 = c1;
		this->c2 = c2;
		this->end = end;
		this->step = step;
	}
	BezierLerper(glm::vec3 start, glm::vec3 end, glm::vec3 c1, glm::vec3 c2, float step, float wait)
	{
		this->start = start;
		this->c1 = c1;
		this->c2 = c2;
		this->end = end;
		this->step = step;
		this->wait = wait;
	}

	glm::vec3 lerpStepSmooth(float step)
	{
		if (t < 1)
		{
			t += step;
			float t1 = smoothstep(0, 1, t);
			float ti = 1 - t1;
			float t2 = t1*t1;
			float t3 = t2 * t1;
			return pow(ti, 3) * start + 3 * pow(ti, 2)*t*c1 + 3 * (ti)*t2*c2 + t3*end;
		}
		stepWait(step);
		return end;
	}

	glm::vec3 lerpStep(float step)
	{
		if (t < 1)
		{
			t += step;
			float t1 = t;
			float ti = 1 - t1;
			float t2 = t1*t1;
			float t3 = t2 * t1;
			return pow(ti, 3) * start + 3 * pow(ti, 2)*t*c1 + 3 * (ti)*t2*c2 + t3*end;
		}
		stepWait(step);
		return end;
	}

	
	glm::vec3 lerpStepSmooth()
	{
		return lerpStepSmooth(step);
	}

	glm::vec3 lerpStep()
	{
		return lerpStep(step);
	}
};


struct LerperSequencer
{
public:
	std::vector<Lerper> sequence;
	int currentLerper = 0;

	LerperSequencer() {}

	LerperSequencer(glm::vec3 start, glm::vec3 first, float step, float wait) 
	{
		addLerper(Lerper(start, first, step, wait));
	}

	LerperSequencer(std::vector<Lerper> sequence)
	{
		this->sequence = sequence;
	}

	void addLerper(Lerper lerper)
	{
		sequence.push_back(lerper);
	}
	void addLerper(glm::vec3 next, float step, float wait)
	{
		sequence.push_back(Lerper(sequence.back().getEnd(), next, step, wait));
	}
	void addLerper(glm::vec3 next)
	{
		sequence.push_back(Lerper(sequence.back().getEnd(), next, sequence.front().getStep(), sequence.front().getWait()));
	}
	void addLerper(glm::vec3 next, float step)
	{
		sequence.push_back(Lerper(sequence.back().getEnd(), next, step, sequence.front().getWait()));
	}
	void addLerper(float wait, glm::vec3 next)
	{
		sequence.push_back(Lerper(sequence.back().getEnd(), next, sequence.front().getStep(), wait));
	}

	//void addLerper(glm::vec3 control, glm::vec3 next)
	//{
	//	sequence.push_back(BezierLerper(sequence.back().getEnd(), control, next, sequence.front().getStep(), sequence.front().getWait()));
	//}

	void reset()
	{
		currentLerper = 0;
		for (Lerper l : sequence)
			l.reset();
	}

	bool isFinished()
	{
		return sequence.size() > 0 && sequence[sequence.size() - 1].isFinished();
	}

	glm::vec3 lerpStep(float step)
	{
		if (sequence.size() > 0)
		{
			if (sequence[currentLerper].isFinished())
			{
				if (currentLerper == sequence.size() - 1)
					return sequence[currentLerper].getEnd();
				else
					currentLerper++;
			}
			return sequence[currentLerper].lerpStep(step);
		}
		return glm::vec3();
	}

	glm::vec3 lerpStepSmooth(float step)
	{
		if (sequence.size() > 0)
		{
			if (sequence[currentLerper].isFinished())
			{
				if (currentLerper == sequence.size() - 1)
					return sequence[currentLerper].getEnd();
				else
					currentLerper++;				
			}
			return sequence[currentLerper].lerpStepSmooth(step);
		}
		return glm::vec3();
	}

	glm::vec3 lerpStepSmooth()
	{
		if (sequence.size() > 0)
		{
			if (sequence[currentLerper].isFinished())
			{
				if (currentLerper == sequence.size() - 1)
					return sequence[currentLerper].getEnd();
				else
					currentLerper++;
			}
			return sequence[currentLerper].lerpStepSmooth();
		}
		return glm::vec3();
	}

	glm::vec3 lerpStep()
	{
		if (sequence.size() > 0)
		{
			if (sequence[currentLerper].isFinished())
			{
				if (currentLerper == sequence.size() - 1)
					return sequence[currentLerper].getEnd();
				else
					currentLerper++;
			}
			return sequence[currentLerper].lerpStep();
		}
		return glm::vec3();
	}

	void next()
	{
		if (sequence.size() > 0)
			if (currentLerper < sequence.size() - 1)
				currentLerper++;
	}

};


struct CameraSequencer : LerperSequencer
{
public:
	std::vector<Lerper> directions;
	std::vector<Lerper> ups;
	std::vector<Lerper> others;

	glm::vec3 currentPosition, currentDirection, currentUp, currentOther;

	CameraSequencer() : LerperSequencer() {}

	CameraSequencer(
		glm::vec3 startPos, glm::vec3 endPos,	glm::vec3 startDir, glm::vec3 endDir, glm::vec3 startUp, glm::vec3 endUp, glm::vec3 startOther, glm::vec3 endOther, float step, float wait)
	{
		addLerper(Lerper(startPos, endPos, step, wait), Lerper(startDir, endDir, step, wait), Lerper(startUp, endUp, step, wait), Lerper(startOther, endOther, step, wait));
	}

	void addLerper(Lerper position, Lerper direction, Lerper up, Lerper other)
	{
		sequence.push_back(position);
		directions.push_back(direction);
		ups.push_back(up);
		others.push_back(other);
	}
	void addLerper(glm::vec3 nextPos, glm::vec3 nextDir, glm::vec3 nextUp, float step, float wait)
	{
		addLerper(Lerper(sequence.back().getEnd(), nextPos, step, wait), Lerper(directions.back().getEnd(), nextDir, step, wait), Lerper(ups.back().getEnd(), nextUp, step, wait), Lerper(others.back().getEnd(), ups.back().getEnd(), step, wait));
	}
	void addLerper(glm::vec3 nextPos, glm::vec3 nextDir, glm::vec3 nextUp, glm::vec3 nextOther, float step, float wait)
	{
		addLerper(Lerper(sequence.back().getEnd(), nextPos, step, wait), Lerper(directions.back().getEnd(), nextDir, step, wait), Lerper(ups.back().getEnd(), nextUp, step, wait), Lerper(others.back().getEnd(), nextOther, step, wait));
	}

	//void addLerper(glm::vec3 nextPos, glm::vec3 control, glm::vec3 nextDir, glm::vec3 nextUp, float step, float wait)
	//{
	//	addLerper(BezierLerper(sequence.back().getEnd(), control, nextPos, step, wait), Lerper(directions.back().getEnd(), nextDir, step, wait), Lerper(ups.back().getEnd(), nextUp, step, wait), Lerper(ups.back().getEnd(), ups.back().getEnd(), step, wait));
	//}

	void reset()
	{
		currentLerper = 0;
		for (int i = 0; i < sequence.size(); ++i)
			sequence[i].reset();
		for (int i = 0; i < directions.size(); ++i)
			directions[i].reset();
		for (int i = 0; i < ups.size(); ++i)
			ups[i].reset();
		for (int i = 0; i < others.size(); ++i)
			others[i].reset();
		if (sequence.size() > 0)
		{
			currentPosition = sequence[currentLerper].getStart();
			currentDirection = directions[currentLerper].getStart();
			currentUp = ups[currentLerper].getStart();
			currentOther = others[currentLerper].getStart();
		}
	}

	bool lerpStep(float step)
	{
		bool finished = false;
		if (sequence.size() > 0)
		{
			if (sequence[currentLerper].isFinished())
			{
				if (currentLerper == sequence.size() - 1)
				{
					currentPosition = sequence[currentLerper].getEnd();
					currentDirection = directions[currentLerper].getEnd();
					currentUp = ups[currentLerper].getEnd();
					currentOther = others[currentLerper].getEnd();
				}
				else
					currentLerper++;
				finished = true;
			}
			else {
				currentPosition = sequence[currentLerper].lerpStep(step);
				currentDirection = directions[currentLerper].lerpStep(step);
				currentUp = ups[currentLerper].lerpStep(step);
				currentOther = others[currentLerper].lerpStep(step);
			}
		}
		else
			currentPosition = currentDirection = currentUp = currentOther = glm::vec3();
		return finished;
	}

	bool lerpStepSmooth(float step)
	{
		bool finished = false;
		if (sequence.size() > 0)
		{
			if (sequence[currentLerper].isFinished())
			{
				if (currentLerper == sequence.size() - 1)
				{
					currentPosition = sequence[currentLerper].getEnd();
					currentDirection = directions[currentLerper].getEnd();
					currentUp = ups[currentLerper].getEnd();
					currentOther = others[currentLerper].getEnd();
				}
				else
					currentLerper++;
				finished = true;
			}
			else {
				currentPosition = sequence[currentLerper].lerpStepSmooth(step);
				currentDirection = directions[currentLerper].lerpStepSmooth(step);
				currentUp = ups[currentLerper].lerpStepSmooth(step);
				currentOther = others[currentLerper].lerpStepSmooth(step);
			}
		}
		else
			currentPosition = currentDirection = currentUp = currentOther = glm::vec3();
		return finished;
	}

	bool lerpStep()
	{
		bool finished = false;
		if (sequence.size() > 0)
		{
			if (sequence[currentLerper].isFinished())
			{
				if (currentLerper == sequence.size() - 1)
				{
					currentPosition = sequence[currentLerper].getEnd();
					currentDirection = directions[currentLerper].getEnd();
					currentUp = ups[currentLerper].getEnd();
					currentOther = others[currentLerper].getEnd();
				}
				else
					currentLerper++;
				finished = true;
			}
			else {
				currentPosition = sequence[currentLerper].lerpStep();
				currentDirection = directions[currentLerper].lerpStep();
				currentUp = ups[currentLerper].lerpStep();
				currentOther = others[currentLerper].lerpStep();
			}
		}
		else
			currentPosition = currentDirection = currentUp = currentOther = glm::vec3();
		return finished;
	}

	bool lerpStepSmooth()
	{
		bool finished = false;
		if (sequence.size() > 0)
		{
			if (sequence[currentLerper].isFinished())
			{
				if (currentLerper == sequence.size() - 1)
				{
					currentPosition = sequence[currentLerper].getEnd();
					currentDirection = directions[currentLerper].getEnd();
					currentUp = ups[currentLerper].getEnd();
					currentOther = others[currentLerper].getEnd();
				}
				else
					currentLerper++;
				finished = true;
			}
			else {
				currentPosition = sequence[currentLerper].lerpStepSmooth();
				currentDirection = directions[currentLerper].lerpStepSmooth();
				currentUp = ups[currentLerper].lerpStepSmooth();
				currentOther = others[currentLerper].lerpStepSmooth();
			}
		}
		else
			currentPosition = currentDirection = currentUp = currentOther = glm::vec3();
		return finished;
	}

private:

};
