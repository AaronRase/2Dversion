#ifndef DW2D_H //Usual macro guard to prevent multiple inclusion
#define DW2D_H

/* This file is part of CosmoLattice, available at www.cosmolattice.net .
   Copyright Daniel G. Figueroa, Adrien Florio, Francisco Torrenti and Wessel Valkenburg.
   Released under the MIT license, see LICENSE.md. */

// File info: Main contributor(s): Daniel G. Figueroa, Adrien Florio, Francisco Torrenti,  Year: 2020

#include "CosmoInterface/cosmointerface.h"

//Include cosmointerface to have access to all of the library.

namespace TempLat
{

    struct ModelPars : public TempLat::DefaultModelPars {
    	static constexpr size_t NScalars = 1;
        // In our phi4 example, we only want 2 scalar fields.
        static constexpr size_t NPotTerms = 3;
        // Z2 part and temperature dependent part.
        static constexpr size_t NDim = 2;
    };

  #define MODELNAME DW2D
  // Here we define the name of the model. This should match the name of your file.

  template<class R>
  using Model = MakeModel(R, ModelPars);
 
  class MODELNAME : public Model<MODELNAME>
 
  {

private:

  double lambda, g, Ti, epsilon;

  public:

    MODELNAME(ParameterParser& parser, RunParameters<double>& runPar, std::shared_ptr<MemoryToolBox> toolBox): //Constructor of our model.
    Model<MODELNAME>(parser,runPar.getLatParams(), toolBox, runPar.dt, STRINGIFY(MODELLABEL)) //MODELLABEL is defined in the cmake.
    {

      lambda = parser.get<double>("lambda");
      g = parser.get<double>("g");
      Ti = parser.get<double>("Ti");
      epsilon = parser.get<double>("epsilon");


        fldS0 = parser.get<double,1>("initial_amplitudes");
        piS0 = parser.get<double,1>("initial_momenta", {0});

        
		//We will make it possible to define the rescaling parameters in the input file.
        alpha = parser.get<double>("alpha"); 
        fStar = parser.get<double>("fStar");
        omegaStar = parser.get<double>("omegaStar");

        setInitialPotentialAndMassesFromPotential();

        /*
          masses2S = {..., ...};
          setInitialPotentialFromPotential();
         */
    }

   /////////
   // Program potential (add as many functions as terms are in the potential)
   /////////

    auto potentialTerms(Tag<0>) // Z2 part
    //
    // Now we need to define the physics of the model. We start by defining the potential.
    // We need to specify as  many potential  terms as we specified in the ModelParams,
    // here 2. Then for every potential terms, we define a function
    //' auto potentialTerms(Tag<N>)'  with N =0,...,NPot -1. The type 'Tag<N>' simply allows
    // to define different function with the same name. The 'auto' keyword lets the compiler
    // figure out on itself what is the actual return type of the function.
    {
        return pow<2>(fStar/omegaStar) * 0.25 * lambda * pow<2>(pow<2>(fldS(0_c))-1);
        // Some notations.  The scalar fields are stored in a collection called 'fldS'.
        // The scalar fields are labelled  from 0 to Ns-1. The field say number 1 is
        // accessed through the syntax 'fldS(0_c)'. The function 'pow<N>(x)'. Works with the
        // known-at-compile-time integer N and compute the expression x*...*x N times.
        // If you don't know the integer at compile time or you don't have an integer,
        // use the more usual syntax pow(x, N).
        // These 'pow' functions are just one example of the many algebraic functions which
        // can be applied to our fields,  see the manual for an exhaustive list
        // and what to do if you want to implement a new one.
    }
    auto potentialTerms(Tag<1>) // Temperature part
    {
        return g * pow<2>(Ti/aI/omegaStar) * pow<2>(fldS(0_c));
    }

    auto potentialTerms(Tag<2>) // Bias part
    {
    	return pow<2>(fStar/omegaStar) * epsilon * lambda * fldS(0_c) * (0.3333 * pow<2>(fldS(0_c)) - 1);
    }
	
   /////////
   // Derivatives of the program potential with respect fields
   // (add one function for each field).
   /////////

    auto potDeriv(Tag<0>) // Derivative with respect scalar field.
    // In exactly the same fashion, we  need to define one derivative of the potential
    // per scalar field (1 in this case).  The integer in Tag<0> tells you the field with
    // respect to which you are defining the derivative of the potential of.
    {
      return  pow<2>(fStar/omegaStar) * lambda * fldS(0_c) * (pow<2>(fldS(0_c))-1) + 2*g * pow<2>(Ti/aI/omegaStar) * fldS(0_c) + pow<2>(fStar/omegaStar) * epsilon * lambda * (pow<2>(fldS(0_c))-1);
    }
	
   /////////
   //  Second derivatives of the program potential with respect fields
   // (add one function for each field)
   /////////

    auto potDeriv2(Tag<0>) // Second derivative with respect scalar field.
    // Finally, for the purpose of initializing the masses, the user needs to define
    // in the same fashion the second derivatives of the potential
    // (put 'return 0' if you are not using this feature).
    {
      return  pow<2>(fStar/omegaStar) * 3*lambda * (pow<2>(fldS(0_c))-0.33) + 2*g * pow<2>(Ti/aI/omegaStar) + pow<2>(fStar/omegaStar) * 2 * epsilon * lambda * fldS(0_c);
    }
		
	
    };
}

#endif //DW2D_H
