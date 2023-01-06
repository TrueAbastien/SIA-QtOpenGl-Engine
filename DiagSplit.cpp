
#include "DiagSplit.h"

void DiagSplit::dispatch(QuadDice::SubPatch& sub, QuadDice::EdgeFactors& ef)
{
	subpatches_quad.push_back(sub);
	edgefactors_quad.push_back(ef);
}

float3 DiagSplit::project(Patch *patch, float2 uv)
{
	float3 P;

	patch->eval(&P, NULL, NULL, uv.x, uv.y);
	if(params.camera)
		P = transform_perspective(&params.camera->worldtoraster, P);

	return P;
}

int DiagSplit::T(Patch *patch, float2 Pstart, float2 Pend)
{
	float3 Plast = make_float3(0.0f, 0.0f, 0.0f);
	float Lsum = 0.0f;
	float Lmax = 0.0f;

	for(int i = 0; i < params.test_steps; i++) {
		float t = i/(float)(params.test_steps-1);

		float3 P = project(patch, Pstart + t*(Pend - Pstart));

		if(i > 0) {
			float L = len(P - Plast);
			Lsum += L;
			Lmax = max(L, Lmax);
		}

		Plast = P;
	}

	int tmin = (int)ceil(Lsum/params.dicing_rate);
	int tmax = (int)ceil((params.test_steps-1)*Lmax/params.dicing_rate); // XXX paper says N instead of N-1, seems wrong?

	if(tmax - tmin > params.split_threshold)
		return DSPLIT_NON_UNIFORM;
	
	return tmax;
}

void DiagSplit::partition_edge(Patch *patch, float2 *P, int *t0, int *t1, float2 Pstart, float2 Pend, int t)
{
	if(t == DSPLIT_NON_UNIFORM) {
		*P = (Pstart + Pend)*0.5f;
		*t0 = T(patch, Pstart, *P);
		*t1 = T(patch, *P, Pend);
	}
	else {
		int I = (int)floor((float)t*0.5f);
		*P = interp(Pstart, Pend, (t == 0)? 0: I/(float)t); /* XXX is t faces or verts */
		*t0 = I;
		*t1 = t - I;
	}
}

void DiagSplit::split(QuadDice::SubPatch& sub, QuadDice::EdgeFactors& ef, int depth)
{
	if((ef.tu0 == DSPLIT_NON_UNIFORM || ef.tu1 == DSPLIT_NON_UNIFORM)) {
		/* partition edges */
		QuadDice::EdgeFactors ef0, ef1;
		float2 Pu0, Pu1;

		partition_edge(sub.patch,
			&Pu0, &ef0.tu0, &ef1.tu0, sub.P00, sub.P10, ef.tu0);
		partition_edge(sub.patch,
			&Pu1, &ef0.tu1, &ef1.tu1, sub.P01, sub.P11, ef.tu1);

		/* split */
		int tsplit = T(sub.patch, Pu0, Pu1);
		ef0.tv0 = ef.tv0;
		ef0.tv1 = tsplit;

		ef1.tv0 = tsplit;
		ef1.tv1 = ef.tv1;

		/* create subpatches */
		QuadDice::SubPatch sub0 = {sub.patch, sub.P00, Pu0, sub.P01, Pu1};
		QuadDice::SubPatch sub1 = {sub.patch, Pu0, sub.P10, Pu1, sub.P11};

		split(sub0, ef0, depth+1);
		split(sub1, ef1, depth+1);
	}
	else if(ef.tv0 == DSPLIT_NON_UNIFORM || ef.tv1 == DSPLIT_NON_UNIFORM) {
		/* partition edges */
		QuadDice::EdgeFactors ef0, ef1;
		float2 Pv0, Pv1;

		partition_edge(sub.patch,
			&Pv0, &ef0.tv0, &ef1.tv0, sub.P00, sub.P01, ef.tv0);
		partition_edge(sub.patch,
			&Pv1, &ef0.tv1, &ef1.tv1, sub.P10, sub.P11, ef.tv1);

		/* split */
		int tsplit = T(sub.patch, Pv0, Pv1);
		ef0.tu0 = ef.tu0;
		ef0.tu1 = tsplit;

		ef1.tu0 = tsplit;
		ef1.tu1 = ef.tu1;

		/* create subpatches */
		QuadDice::SubPatch sub0 = {sub.patch, sub.P00, sub.P10, Pv0, Pv1};
		QuadDice::SubPatch sub1 = {sub.patch, Pv0, Pv1, sub.P01, sub.P11};

		split(sub0, ef0, depth+1);
		split(sub1, ef1, depth+1);
	}
	else
		dispatch(sub, ef);
}

void DiagSplit::split_quad(Patch *patch)
{
	QuadDice::SubPatch sub_split;
	QuadDice::EdgeFactors ef_split;

	sub_split.patch = patch;
	sub_split.P00 = make_float2(0.0f, 0.0f);
	sub_split.P10 = make_float2(1.0f, 0.0f);
	sub_split.P01 = make_float2(0.0f, 1.0f);
	sub_split.P11 = make_float2(1.0f, 1.0f);

	ef_split.tu0 = T(patch, sub_split.P00, sub_split.P10);
	ef_split.tu1 = T(patch, sub_split.P01, sub_split.P11);
	ef_split.tv0 = T(patch, sub_split.P00, sub_split.P01);
	ef_split.tv1 = T(patch, sub_split.P10, sub_split.P11);

	split(sub_split, ef_split);

	QuadDice dice(params);

	for(size_t i = 0; i < subpatches_quad.size(); i++) {
		QuadDice::SubPatch& sub = subpatches_quad[i];
		QuadDice::EdgeFactors& ef = edgefactors_quad[i];

		ef.tu0 = max(ef.tu0, 1);
		ef.tu1 = max(ef.tu1, 1);
		ef.tv0 = max(ef.tv0, 1);
		ef.tv1 = max(ef.tv1, 1);

		dice.dice(sub, ef);
	}

	subpatches_quad.clear();
	edgefactors_quad.clear();
}
