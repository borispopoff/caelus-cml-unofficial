volScalarField const R = ( thermo.Cp() - thermo.Cv() )();

h = thermo.Cp()/thermo.Cv()*(rhoE/rho - 0.5*magSqr(U) - turbulence->k());

h.correctBoundaryConditions();

p = (1.0 - thermo.Cv()/thermo.Cp())*rho*h;

p.correctBoundaryConditions();

thermo.correct();

rho.boundaryFieldRef() = thermo.rho()().boundaryField();

rhoU.boundaryFieldRef() = rho.boundaryField()*U.boundaryField();

rhoE.boundaryFieldRef() = rho.boundaryField()
                     * (h.boundaryField() + 0.5*magSqr(U.boundaryField())) 
                     - p.boundaryField();

T = thermo.T();
T.correctBoundaryConditions();
