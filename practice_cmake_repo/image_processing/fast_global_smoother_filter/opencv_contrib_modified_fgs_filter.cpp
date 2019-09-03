/*
 *  By downloading, copying, installing or using the software you agree to this license.
 *  If you do not agree to this license, do not download, install,
 *  copy or use the software.
 *
 *
 *  License Agreement
 *  For Open Source Computer Vision Library
 *  (3 - clause BSD License)
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met :
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and / or other materials provided with the distribution.
 *
 *  * Neither the names of the copyright holders nor the names of the contributors
 *  may be used to endorse or promote products derived from this software
 *  without specific prior written permission.
 *
 *  This software is provided by the copyright holders and contributors "as is" and
 *  any express or implied warranties, including, but not limited to, the implied
 *  warranties of merchantability and fitness for a particular purpose are disclaimed.
 *  In no event shall copyright holders or contributors be liable for any direct,
 *  indirect, incidental, special, exemplary, or consequential damages
 *  (including, but not limited to, procurement of substitute goods or services;
 *  loss of use, data, or profits; or business interruption) however caused
 *  and on any theory of liability, whether in contract, strict liability,
 *  or tort(including negligence or otherwise) arising in any way out of
 *  the use of this software, even if advised of the possibility of such damage.
 */

#include "stdafx.h"

#include "opencv_contrib_modified_fgs_filter.h"

//#include "precomp.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/core/cvdef.h>
#include <opencv2/core/core_c.h>
//#include <opencv2/core/private.hpp>
#include <opencv2/imgproc.hpp>

//Comment out the following two to disable SIMD in this file. Both results and timings change slightly.
#define CV_CPU_HAS_SUPPORT_SSE2 true
#include "opencv2/core/hal/intrin.hpp"

#include <vector>



namespace cv {
namespace ximgproc_modified {

using std::vector;



/** @brief Interface for implementations of Fast Global Smoother filter.

For more details about this filter see @cite Min2014 and @cite Farbman2008 .
*/
class CV_EXPORTS_W FastGlobalSmootherFilter : public Algorithm
{
public:
    /** @brief Apply smoothing operation to the source image.

    @param src source image for filtering with unsigned 8-bit or signed 16-bit or floating-point 32-bit depth and up to 4 channels.

    @param dst destination image.
    */
    CV_WRAP virtual void filter(InputArray src, OutputArray dst) = 0;
};

/** @brief Factory method, create instance of FastGlobalSmootherFilter and execute the initialization routines.

@param guide image serving as guide for filtering. It should have 8-bit depth and either 1 or 3 channels.

@param lambda parameter defining the amount of regularization

@param sigma_color parameter, that is similar to color space sigma in bilateralFilter.

@param lambda_attenuation internal parameter, defining how much lambda decreases after each iteration. Normally,
it should be 0.25. Setting it to 1.0 may lead to streaking artifacts.

@param num_iter number of iterations used for filtering, 3 is usually enough.

For more details about Fast Global Smoother parameters, see the original paper @cite Min2014. However, please note that
there are several differences. Lambda attenuation described in the paper is implemented a bit differently so do not
expect the results to be identical to those from the paper; sigma_color values from the paper should be multiplied by 255.0 to
achieve the same effect. Also, in case of image filtering where source and guide image are the same, authors
propose to dynamically update the guide image after each iteration. To maximize the performance this feature
was not implemented here.
*/
CV_EXPORTS_W Ptr<FastGlobalSmootherFilter> createFastGlobalSmootherFilter(InputArray guide, double lambda, double sigma_color, double lambda_attenuation=0.25, int num_iter=3, bool zero_norm = false);

/** @brief Simple one-line Fast Global Smoother filter call. If you have multiple images to filter with the same
guide then use FastGlobalSmootherFilter interface to avoid extra computations.

@param guide image serving as guide for filtering. It should have 8-bit depth and either 1 or 3 channels.

@param src source image for filtering with unsigned 8-bit or signed 16-bit or floating-point 32-bit depth and up to 4 channels.

@param dst destination image.

@param lambda parameter defining the amount of regularization

@param sigma_color parameter, that is similar to color space sigma in bilateralFilter.

@param lambda_attenuation internal parameter, defining how much lambda decreases after each iteration. Normally,
it should be 0.25. Setting it to 1.0 may lead to streaking artifacts.

@param num_iter number of iterations used for filtering, 3 is usually enough.
*/
CV_EXPORTS_W void fastGlobalSmootherFilter(InputArray guide, InputArray src, OutputArray dst, double lambda, double sigma_color, double lambda_attenuation=0.25, int num_iter=3, bool zero_norm = false);




//hax
constexpr int WorkVec_type = CV_32FC1;

typedef float WorkType;

inline float get_negative_weight(float p1, float p2, bool zero_norm) {
    //Farbman's weight works better than the one involving exp.
    if (zero_norm)
        return -1.0f / ((p1 - p2)*(p1 - p2) + 1e-7f);
    return -1.0f / (abs(p1 - p2) + 1e-4f);
}




class FastGlobalSmootherFilterImpl : public FastGlobalSmootherFilter
{
public:
    static Ptr<FastGlobalSmootherFilterImpl> create(InputArray guide, double lambda, double sigma_color, int num_iter,double lambda_attenuation, bool zero_norm);
    void filter(InputArray src, OutputArray dst);

protected:
    int w,h;
    int num_stripes;
    float sigmaColor,lambda;
    float lambda_attenuation;
    bool zero_norm;
    int num_iter;
    Mat Chor, Cvert;
    Mat interD;
    void init(InputArray guide,double _lambda,double _sigmaColor,int _num_iter,double _lambda_attenuation, bool _zero_norm);
    void horizontalPass(Mat& cur);
    void verticalPass(Mat& cur);
protected:
    struct HorizontalPass_ParBody : public ParallelLoopBody
    {
        FastGlobalSmootherFilterImpl* fgs;
        Mat* cur;
        int nstripes, stripe_sz;
        int h;

        HorizontalPass_ParBody(FastGlobalSmootherFilterImpl &_fgs, Mat& _cur, int _nstripes, int _h);
        void operator () (const Range& range) const;
    };
    inline void process_4row_block(Mat* cur,int i);
    inline void process_row(Mat* cur,int i);

    struct VerticalPass_ParBody : public ParallelLoopBody
    {
        FastGlobalSmootherFilterImpl* fgs;
        Mat* cur;
        int nstripes, stripe_sz;
        int w;

        VerticalPass_ParBody(FastGlobalSmootherFilterImpl &_fgs, Mat& _cur, int _nstripes, int _w);
        void operator () (const Range& range) const;
    };

    struct ComputeHorizontalWeights_ParBody : public ParallelLoopBody
    {
        FastGlobalSmootherFilterImpl* fgs;
        Mat* guide;
        int nstripes, stripe_sz;
        int h;

        ComputeHorizontalWeights_ParBody(FastGlobalSmootherFilterImpl &_fgs, Mat& _guide, int _nstripes, int _h);
        void operator () (const Range& range) const;
    };

    struct ComputeVerticalWeights_ParBody : public ParallelLoopBody
    {
        FastGlobalSmootherFilterImpl* fgs;
        Mat* guide;
        int nstripes, stripe_sz;
        int w;

        ComputeVerticalWeights_ParBody(FastGlobalSmootherFilterImpl &_fgs, Mat& _guide, int _nstripes, int _w);
        void operator () (const Range& range) const;
    };
};


void FastGlobalSmootherFilterImpl::init(InputArray guide,double _lambda,double _sigmaColor,int _num_iter,double _lambda_attenuation, bool _zero_norm)
{
    CV_Assert( !guide.empty() && _lambda >= 0 && _sigmaColor >= 0 && _num_iter >=1 );
    CV_Assert( guide.depth() == CV_32F && (guide.channels() == 1) );
    sigmaColor = (float)_sigmaColor;
    lambda = (float)_lambda;
    lambda_attenuation = (float)_lambda_attenuation;
    zero_norm = _zero_norm;
    num_iter = _num_iter;
    //TODO: Viktor Y: INVESTIGATE we are using the TBB also !!!
    //num_stripes = getNumThreads();
    num_stripes = 1;
    //int num_levels = 3*256*256;

    w = guide.cols();
    h = guide.rows();
    Chor.  create(h,w,WorkVec_type);
    Cvert. create(h,w,WorkVec_type);
    interD.create(h,w,WorkVec_type);
    Mat guideMat = guide.getMat();

    parallel_for_(Range(0,num_stripes),ComputeHorizontalWeights_ParBody(*this,guideMat,num_stripes,h));
    parallel_for_(Range(0,num_stripes),ComputeVerticalWeights_ParBody  (*this,guideMat,num_stripes,w));
}

Ptr<FastGlobalSmootherFilterImpl> FastGlobalSmootherFilterImpl::create(InputArray guide, double lambda, double sigma_color, int num_iter, double lambda_attenuation, bool zero_norm)
{
    FastGlobalSmootherFilterImpl *fgs = new FastGlobalSmootherFilterImpl();
    fgs->init(guide,lambda,sigma_color,num_iter,lambda_attenuation,zero_norm);
    return Ptr<FastGlobalSmootherFilterImpl>(fgs);
}

void FastGlobalSmootherFilterImpl::filter(InputArray src, OutputArray dst)
{
    CV_Assert(!src.empty() && (src.depth() == CV_8U || src.depth() == CV_16S || src.depth() == CV_32F) && src.channels() <= 4);
    if (src.rows() != h || src.cols() != w)
    {
        CV_Error(Error::StsBadSize, "Size of the filtered image must be equal to the size of the guide image");
        return;
    }


    float lambda_ref = lambda;
    lambda = lambda_ref;

    //Move data in src to dst.
    Mat& dstMat = dst.getMatRef();
    src.getMat().copyTo(dstMat);

    //Smoothen dst in-place.
    for (int n = 0; n < num_iter; n++)
    {
        horizontalPass(dstMat);
        verticalPass(dstMat);
        lambda *= lambda_attenuation;
    }

    lambda = lambda_ref;
}

void FastGlobalSmootherFilterImpl::horizontalPass(Mat& cur)
{
    parallel_for_(Range(0,num_stripes),HorizontalPass_ParBody(*this,cur,num_stripes,h));
}

void FastGlobalSmootherFilterImpl::verticalPass(Mat& cur)
{
    parallel_for_(Range(0,num_stripes),VerticalPass_ParBody(*this,cur,num_stripes,w));
}

FastGlobalSmootherFilterImpl::HorizontalPass_ParBody::HorizontalPass_ParBody(FastGlobalSmootherFilterImpl &_fgs, Mat& _cur, int _nstripes, int _h):
fgs(&_fgs),cur(&_cur), nstripes(_nstripes), h(_h)
{
    stripe_sz = (int)ceil(h/(double)nstripes);
}

void FastGlobalSmootherFilterImpl::HorizontalPass_ParBody::operator()(const Range& range) const
{
    int start = std::min(range.start * stripe_sz, h);
    int end = std::min(range.end   * stripe_sz, h);

    int i = start;
    for (; i < end - 3; i += 4)
        fgs->process_4row_block(cur, i);
    for (; i < end; i++)
        fgs->process_row(cur, i);
}

void FastGlobalSmootherFilterImpl::process_4row_block(Mat* cur,int i)
{
    WorkType denom,denom_next,denom_next2,denom_next3;

    WorkType *Chor_row   = (WorkType*)Chor.ptr  (i);
    WorkType *interD_row = (WorkType*)interD.ptr(i);
    WorkType *cur_row    = (WorkType*)cur->ptr  (i);

    WorkType *Chor_row_next   = (WorkType*)Chor.ptr  (i+1);
    WorkType *interD_row_next = (WorkType*)interD.ptr(i+1);
    WorkType *cur_row_next    = (WorkType*)cur->ptr  (i+1);

    WorkType *Chor_row_next2   = (WorkType*)Chor.ptr  (i+2);
    WorkType *interD_row_next2 = (WorkType*)interD.ptr(i+2);
    WorkType *cur_row_next2    = (WorkType*)cur->ptr  (i+2);

    WorkType *Chor_row_next3   = (WorkType*)Chor.ptr  (i+3);
    WorkType *interD_row_next3 = (WorkType*)interD.ptr(i+3);
    WorkType *cur_row_next3    = (WorkType*)cur->ptr  (i+3);

    float coef_cur,          coef_prev;
    float coef_cur_row_next, coef_prev_row_next;
    float coef_cur_row_next2,coef_prev_row_next2;
    float coef_cur_row_next3,coef_prev_row_next3;

    //forward pass:
    coef_prev           = lambda*Chor_row[0];
    coef_prev_row_next  = lambda*Chor_row_next[0];
    coef_prev_row_next2 = lambda*Chor_row_next2[0];
    coef_prev_row_next3 = lambda*Chor_row_next3[0];

    interD_row[0]       = coef_prev          /(1-coef_prev);
    interD_row_next[0]  = coef_prev_row_next /(1-coef_prev_row_next);
    interD_row_next2[0] = coef_prev_row_next2/(1-coef_prev_row_next2);
    interD_row_next3[0] = coef_prev_row_next3/(1-coef_prev_row_next3);

    cur_row[0]       = cur_row[0]      /(1-coef_prev);
    cur_row_next[0]  = cur_row_next[0] /(1-coef_prev_row_next);
    cur_row_next2[0] = cur_row_next2[0]/(1-coef_prev_row_next2);
    cur_row_next3[0] = cur_row_next3[0]/(1-coef_prev_row_next3);
    int j=1;

#if CV_SIMD128
    {
        v_float32x4 coef_prev_reg(coef_prev,coef_prev_row_next,coef_prev_row_next2,coef_prev_row_next3);
        v_float32x4 interD_prev_reg(interD_row[0],interD_row_next[0],interD_row_next2[0],interD_row_next3[0]);
        v_float32x4 cur_prev_reg(cur_row[0],cur_row_next[0],cur_row_next2[0],cur_row_next3[0]);
        v_float32x4 lambda_reg(lambda,lambda,lambda,lambda);
        v_float32x4 one_reg(1.0f,1.0f,1.0f,1.0f);

        v_float32x4 a0,a1,a2,a3;
        v_float32x4 b0,b1,b2,b3;
        v_float32x4 aux0,aux1,aux2,aux3;

#define PROC4(Chor_in,cur_in,coef_prev_in,interD_prev_in,cur_prev_in,interD_out,cur_out,coef_cur_out)\
        coef_cur_out = lambda_reg*Chor_in;\
        aux0 = interD_prev_in*coef_prev_in;\
        aux1 = coef_cur_out+coef_prev_in;\
        aux1 = one_reg-aux1;\
        aux0 = aux1-aux0;\
        interD_out = coef_cur_out/aux0;\
        aux1 = cur_prev_in*coef_prev_in;\
        aux1 = cur_in - aux1;\
        cur_out = aux1/aux0;

        for(;j<w-3;j+=4)
        {
            // processing a 4x4 block:

            aux0 = v_load(Chor_row+j);
            aux1 = v_load(Chor_row_next+j);
            aux2 = v_load(Chor_row_next2+j);
            aux3 = v_load(Chor_row_next3+j);
            v_transpose4x4(aux0,aux1,aux2,aux3,a0,a1,a2,a3);

            aux0 = v_load(cur_row+j);
            aux1 = v_load(cur_row_next+j);
            aux2 = v_load(cur_row_next2+j);
            aux3 = v_load(cur_row_next3+j);
            v_transpose4x4(aux0,aux1,aux2,aux3,b0,b1,b2,b3);

            PROC4(a0,b0,coef_prev_reg,interD_prev_reg,cur_prev_reg,a0,b0,aux2);
            PROC4(a1,b1,aux2,a0,b0,a1,b1,aux3);
            PROC4(a2,b2,aux3,a1,b1,a2,b2,aux2);
            PROC4(a3,b3,aux2,a2,b2,a3,b3,aux3);

            interD_prev_reg = a3;
            cur_prev_reg = b3;
            coef_prev_reg = aux3;

            v_transpose4x4(a0,a1,a2,a3,aux0,aux1,aux2,aux3);
            v_store(interD_row+j,aux0);
            v_store(interD_row_next+j,aux1);
            v_store(interD_row_next2+j,aux2);
            v_store(interD_row_next3+j,aux3);

            v_transpose4x4(b0,b1,b2,b3,aux0,aux1,aux2,aux3);
            v_store(cur_row+j,aux0);
            v_store(cur_row_next+j,aux1);
            v_store(cur_row_next2+j,aux2);
            v_store(cur_row_next3+j,aux3);
        }
#undef PROC4
    }
#endif

    for(;j<w;j++)
    {
        coef_prev           = lambda*Chor_row[j-1];
        coef_prev_row_next  = lambda*Chor_row_next[j-1];
        coef_prev_row_next2 = lambda*Chor_row_next2[j-1];
        coef_prev_row_next3 = lambda*Chor_row_next3[j-1];

        coef_cur           = lambda*Chor_row[j];
        coef_cur_row_next  = lambda*Chor_row_next[j];
        coef_cur_row_next2 = lambda*Chor_row_next2[j];
        coef_cur_row_next3 = lambda*Chor_row_next3[j];

        denom       = (1-coef_prev          -coef_cur)          -interD_row[j-1]      *coef_prev;
        denom_next  = (1-coef_prev_row_next -coef_cur_row_next) -interD_row_next[j-1] *coef_prev_row_next;
        denom_next2 = (1-coef_prev_row_next2-coef_cur_row_next2)-interD_row_next2[j-1]*coef_prev_row_next2;
        denom_next3 = (1-coef_prev_row_next3-coef_cur_row_next3)-interD_row_next3[j-1]*coef_prev_row_next3;

        interD_row[j]       = coef_cur          /denom;
        interD_row_next[j]  = coef_cur_row_next /denom_next;
        interD_row_next2[j] = coef_cur_row_next2/denom_next2;
        interD_row_next3[j] = coef_cur_row_next3/denom_next3;

        cur_row[j]       = (cur_row[j]      -cur_row[j-1]      *coef_prev)          /denom;
        cur_row_next[j]  = (cur_row_next[j] -cur_row_next[j-1] *coef_prev_row_next) /denom_next;
        cur_row_next2[j] = (cur_row_next2[j]-cur_row_next2[j-1]*coef_prev_row_next2)/denom_next2;
        cur_row_next3[j] = (cur_row_next3[j]-cur_row_next3[j-1]*coef_prev_row_next3)/denom_next3;
    }
    //backward pass:
    j = w-2;

#if CV_SIMD128
    {
        v_float32x4 cur_next_reg(cur_row[w-1],cur_row_next[w-1],cur_row_next2[w-1],cur_row_next3[w-1]);
        v_float32x4 a0,a1,a2,a3;
        v_float32x4 b0,b1,b2,b3;
        v_float32x4 aux0,aux1,aux2,aux3;
        for(j-=3;j>=0;j-=4)
        {
            //process 4x4 block:

            aux0 = v_load(interD_row+j);
            aux1 = v_load(interD_row_next+j);
            aux2 = v_load(interD_row_next2+j);
            aux3 = v_load(interD_row_next3+j);
            v_transpose4x4(aux0,aux1,aux2,aux3,a0,a1,a2,a3);

            aux0 = v_load(cur_row+j);
            aux1 = v_load(cur_row_next+j);
            aux2 = v_load(cur_row_next2+j);
            aux3 = v_load(cur_row_next3+j);
            v_transpose4x4(aux0,aux1,aux2,aux3,b0,b1,b2,b3);

            aux0 = a3*cur_next_reg;
            b3 = b3-aux0;
            aux0 = a2*b3;
            b2 = b2-aux0;
            aux0 = a1*b2;
            b1 = b1-aux0;
            aux0 = a0*b1;
            b0 = b0-aux0;

            cur_next_reg = b0;

            v_transpose4x4(b0,b1,b2,b3,aux0,aux1,aux2,aux3);
            v_store(cur_row+j,aux0);
            v_store(cur_row_next+j,aux1);
            v_store(cur_row_next2+j,aux2);
            v_store(cur_row_next3+j,aux3);
        }
        j+=3;
    }
#endif

    for(;j>=0;j--)
    {
        cur_row[j]       = cur_row[j]      -interD_row[j]      *cur_row[j+1];
        cur_row_next[j]  = cur_row_next[j] -interD_row_next[j] *cur_row_next[j+1];
        cur_row_next2[j] = cur_row_next2[j]-interD_row_next2[j]*cur_row_next2[j+1];
        cur_row_next3[j] = cur_row_next3[j]-interD_row_next3[j]*cur_row_next3[j+1];
    }
}

void FastGlobalSmootherFilterImpl::process_row(Mat* cur,int i)
{
    WorkType denom;
    WorkType *Chor_row = (WorkType*)Chor.ptr(i);
    WorkType *interD_row = (WorkType*)interD.ptr(i);
    WorkType *cur_row = (WorkType*)cur->ptr(i);

    float coef_cur,coef_prev;

    //forward pass:
    coef_prev = lambda*Chor_row[0];
    interD_row[0] = coef_prev/(1-coef_prev);
    cur_row[0] = cur_row[0]/(1-coef_prev);

    for(int j=1;j<w;j++)
    {
        coef_cur = lambda*Chor_row[j];
        denom = (1-coef_prev-coef_cur)-interD_row[j-1]*coef_prev;
        interD_row[j] = coef_cur/denom;
        cur_row[j] = (cur_row[j]-cur_row[j-1]*coef_prev)/denom;
        coef_prev = coef_cur;
    }

    //backward pass:
    for(int j=w-2;j>=0;j--)
        cur_row[j] = cur_row[j]-interD_row[j]*cur_row[j+1];
}

FastGlobalSmootherFilterImpl::VerticalPass_ParBody::VerticalPass_ParBody(FastGlobalSmootherFilterImpl &_fgs, Mat& _cur, int _nstripes, int _w):
fgs(&_fgs),cur(&_cur), nstripes(_nstripes), w(_w)
{
    stripe_sz = (int)ceil(w/(double)nstripes);
}

void FastGlobalSmootherFilterImpl::VerticalPass_ParBody::operator()(const Range& range) const
{
    int start = std::min(range.start * stripe_sz, w);
    int end   = std::min(range.end   * stripe_sz, w);

    //float lambda = fgs->lambda;
    WorkType denom;
    WorkType *Cvert_row, *Cvert_row_prev;
    WorkType *interD_row, *interD_row_prev, *cur_row, *cur_row_prev, *cur_row_next;

    float coef_cur,coef_prev;

    Cvert_row = (WorkType*)fgs->Cvert.ptr(0);
    interD_row = (WorkType*)fgs->interD.ptr(0);
    cur_row = (WorkType*)cur->ptr(0);

    //INFO: for single threaded implementation start = 0; end = width
    //forward pass:
    for(int j=start;j<end;j++)
    {
        coef_cur = fgs->lambda*Cvert_row[j];
        interD_row[j] = coef_cur/(1-coef_cur);
        cur_row[j] = cur_row[j]/(1-coef_cur);
    }

    for(int i=1;i<fgs->h;i++)
    {
        Cvert_row = (WorkType*)fgs->Cvert.ptr(i);
        Cvert_row_prev = (WorkType*)fgs->Cvert.ptr(i-1);
        interD_row = (WorkType*)fgs->interD.ptr(i);
        interD_row_prev = (WorkType*)fgs->interD.ptr(i-1);
        cur_row = (WorkType*)cur->ptr(i);
        cur_row_prev = (WorkType*)cur->ptr(i-1);
        int j = start;

#if CV_SIMD128
        v_float32x4 a,b,c,d,coef_cur_reg,coef_prev_reg;
        v_float32x4 one_reg(1.0f,1.0f,1.0f,1.0f);
        v_float32x4 lambda_reg(fgs->lambda,fgs->lambda,fgs->lambda,fgs->lambda);
        int sz4 = 4*((end-start)/4);
        int end4 = start+sz4;
        for(;j<end4;j+=4)
        {
            a = v_load(Cvert_row_prev+j);
            b = v_load(Cvert_row+j);
            coef_prev_reg = lambda_reg*a;
            coef_cur_reg =  lambda_reg*b;

            a = v_load(interD_row_prev+j);
            a = a*coef_prev_reg;

            b = coef_prev_reg+coef_cur_reg;
            b = b+a;
            a = one_reg-b; //computed denom

            b =  coef_cur_reg/a; //computed interD_row

            c = v_load(cur_row_prev+j);
            c = c*coef_prev_reg;

            d = v_load(cur_row+j);
            d = d-c;
            d = d/a; //computed cur_row

            v_store(interD_row+j,b);
            v_store(cur_row+j,d);
        }
#endif
        for(;j<end;j++)
        {
            coef_prev = fgs->lambda*Cvert_row_prev[j];
            coef_cur  = fgs->lambda*Cvert_row[j];
            denom = (1-coef_prev-coef_cur)-interD_row_prev[j]*coef_prev;
            interD_row[j] = coef_cur/denom;
            cur_row[j] = (cur_row[j]-cur_row_prev[j]*coef_prev)/denom;
        }
    }

    //backward pass:
    for(int i=fgs->h-2;i>=0;i--)
    {
        interD_row = (WorkType*)fgs->interD.ptr(i);
        cur_row = (WorkType*)cur->ptr(i);
        cur_row_next = (WorkType*)cur->ptr(i+1);
        int j = start;
#if CV_SIMD128
        v_float32x4 a,b;
        int sz4 = 4*((end-start)/4);
        int end4 = start+sz4;
        for(;j<end4;j+=4)
        {
            a = v_load(interD_row+j);
            b = v_load(cur_row_next+j);
            b = a*b;

            a = v_load(cur_row+j);
            b = a-b;
            v_store(cur_row+j,b);
        }
#endif
        for(;j<end;j++)
            cur_row[j] = cur_row[j]-interD_row[j]*cur_row_next[j];
    }
}

FastGlobalSmootherFilterImpl::ComputeHorizontalWeights_ParBody::ComputeHorizontalWeights_ParBody(FastGlobalSmootherFilterImpl &_fgs, Mat& _guide, int _nstripes, int _h):
fgs(&_fgs),guide(&_guide), nstripes(_nstripes), h(_h)
{
    stripe_sz = (int)ceil(h/(double)nstripes);
}

void FastGlobalSmootherFilterImpl::ComputeHorizontalWeights_ParBody::operator()(const Range& range) const
{
    int start = std::min(range.start * stripe_sz, h);
    int end = std::min(range.end   * stripe_sz, h);

    //TODO: might need refactor if it's not multithreaded
    const bool zero_norm_local = fgs->zero_norm;

    float *row;
    WorkType *Chor_row;

    for (int i = start; i < end; i++)
    {
        row = guide->ptr<float>(i);
        Chor_row = (WorkType*)fgs->Chor.ptr(i);
        Chor_row[0] = get_negative_weight(row[0], row[1], zero_norm_local);
        row += 1;
        for (int j = 1; j < fgs->w - 1; j++)
        {
            Chor_row[j] = get_negative_weight(row[0], row[1], zero_norm_local);
            row += 1;
        }
        Chor_row[fgs->w - 1] = 0;
    }
}

FastGlobalSmootherFilterImpl::ComputeVerticalWeights_ParBody::ComputeVerticalWeights_ParBody(FastGlobalSmootherFilterImpl &_fgs, Mat& _guide, int _nstripes, int _w):
fgs(&_fgs),guide(&_guide), nstripes(_nstripes), w(_w)
{
    stripe_sz = (int)ceil(w/(double)nstripes);
}

void FastGlobalSmootherFilterImpl::ComputeVerticalWeights_ParBody::operator()(const Range& range) const
{
    int start = std::min(range.start * stripe_sz, w);
    int end   = std::min(range.end   * stripe_sz, w);

    const bool zero_norm_local = fgs->zero_norm;

    float *row,*row_next;
    WorkType *Cvert_row;

    Cvert_row = (WorkType*)fgs->Cvert.ptr(0);
    row = guide->ptr<float>(0)+start*1;
    row_next = guide->ptr<float>(1)+start*1;
    for(int j=start;j<end;j++)
    {
        Cvert_row[j] = get_negative_weight(row[0], row_next[0], zero_norm_local);
        row+=1;
        row_next+=1;
    }

    for(int i=1;i<fgs->h-1;i++)
    {
        row = guide->ptr<float>(i)+start*1;
        row_next = guide->ptr<float>(i+1)+start*1;
        Cvert_row = (WorkType*)fgs->Cvert.ptr(i);
        for(int j=start;j<end;j++)
        {
            Cvert_row[j] = get_negative_weight(row[0], row_next[0], zero_norm_local);
            row+=1;
            row_next+=1;
        }
    }

    Cvert_row = (WorkType*)fgs->Cvert.ptr(fgs->h-1);
    for(int j=start;j<end;j++)
        Cvert_row[j] = 0;
}




////////////////////////////////////////////////////////////////////////////////////////////////

CV_EXPORTS_W
Ptr<FastGlobalSmootherFilter> createFastGlobalSmootherFilter(InputArray guide, double lambda, double sigma_color, double lambda_attenuation, int num_iter, bool zero_norm)
{
    return Ptr<FastGlobalSmootherFilter>(FastGlobalSmootherFilterImpl::create(guide, lambda, sigma_color, num_iter, lambda_attenuation, zero_norm));
}

CV_EXPORTS_W
void fastGlobalSmootherFilter(InputArray guide, InputArray src, OutputArray dst, double lambda, double sigma_color, double lambda_attenuation, int num_iter, bool zero_norm)
{
    Ptr<FastGlobalSmootherFilter> fgs = createFastGlobalSmootherFilter(guide, lambda, sigma_color, lambda_attenuation, num_iter, zero_norm);
    fgs->filter(src, dst);
}

}
}



void opencv_contrib_modified_fastGlobalSmootherFilter(const cv::Mat &src, cv::Mat &dst, float lambda, size_t iterates, size_t reweighting_iterates, bool zero_norm) {
    using namespace cv;

    //Use a pointer to select full or halfsize src and dst.
    Mat src_half, dst_half;

    //Init Dst to Src. This is for iterative reweighing, otherwise one call of fastGlobalSmootherFilter could use src as the guide.
    src.copyTo(dst);

#if 1
    for (size_t reweighting_iterate = 0; reweighting_iterate != reweighting_iterates; reweighting_iterate++)
        //Iterative reweighting is accomplished by using dst image as "guide", ie the image used to compute smoothing weights. src is always used for the data term.
        ximgproc_modified::fastGlobalSmootherFilter(dst, src, dst, (double)lambda, 1e200, 0.25, (int)iterates, zero_norm);

#else
    constexpr const double SIGMA_COLOR = 1e200;
    constexpr const double LAMDA_ATTENUATION = 0.25;

    Ptr<cv::ximgproc_modified::FastGlobalSmootherFilter> fgs = cv::ximgproc_modified::createFastGlobalSmootherFilter(dst, static_cast<double>(lambda),
                                                                        SIGMA_COLOR,
                                                                        LAMDA_ATTENUATION,
                                                                        static_cast<int>(iterates),
                                                                        zero_norm);
    for (size_t reweighting_iterate = 0; reweighting_iterate < reweighting_iterates; ++reweighting_iterate) {
        fgs->filter(src, dst);
    }
#endif
}
